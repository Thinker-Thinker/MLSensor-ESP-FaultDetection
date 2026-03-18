#ifndef ESP_FEATURES_H
#define ESP_FEATURES_H

#include "esp_scaler.h"
#include <math.h>
#include <string.h>

#define WINDOW_SIZE     20      /* timesteps per inference window */
#define N_SIGNALS       6       /* number of sensor channels     */
#define STEP_SIZE       10      /* window stride in timesteps    */

/* Signal channel indices */
#define CH_I_SENSOR     0
#define CH_OMEGA        1
#define CH_Q_SENSOR     2
#define CH_H_SENSOR     3
#define CH_VIB_SENSOR   4
#define CH_SLIP_SENSOR  5

/* Fault class labels */
#define CLASS_NORMAL    0
#define CLASS_GASLOCK   1
#define CLASS_EROSION   2
#define CLASS_FRICTION  3

static const char* CLASS_NAMES[] = {
    "NORMAL", "GAS_LOCK", "EROSION", "FRICTION"
};

/* ── Feature vector buffer ───────────────────────────────── */
static float feature_vector[N_FEATURES];

/* ── Helper: mean ────────────────────────────────────────── */
static float compute_mean(float* x, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) sum += x[i];
    return sum / n;
}

/* ── Helper: std ─────────────────────────────────────────── */
static float compute_std(float* x, int n, float mean) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) sum += (x[i]-mean)*(x[i]-mean);
    return sqrtf(sum / n);
}

/* ── Helper: kurtosis ────────────────────────────────────── */
static float compute_kurtosis(float* x, int n, float mean, float std) {
    if (std < 1e-9f) return 0.0f;
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        float z = (x[i] - mean) / std;
        sum += z * z * z * z;
    }
    return (sum / n) - 3.0f;  /* excess kurtosis */
}

/* ── Helper: skewness ────────────────────────────────────── */
static float compute_skewness(float* x, int n, float mean, float std) {
    if (std < 1e-9f) return 0.0f;
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        float z = (x[i] - mean) / std;
        sum += z * z * z;
    }
    return sum / n;
}

/* ── Helper: peak ────────────────────────────────────────── */
static float compute_peak(float* x, int n) {
    float peak = 0.0f;
    for (int i = 0; i < n; i++) {
        float ax = fabsf(x[i]);
        if (ax > peak) peak = ax;
    }
    return peak;
}

/*
 * extract_features()
 * Fills feature_vector[] from a WINDOW_SIZE x N_SIGNALS buffer.
 * window[sample][channel] layout.
 * Returns pointer to feature_vector for convenience.
 */
static float* extract_features(float window[WINDOW_SIZE][N_SIGNALS]) {

    int feat_idx = 0;

    /* ── Per-signal statistical features ─────────────────── */
    for (int ch = 0; ch < N_SIGNALS; ch++) {
        float x[WINDOW_SIZE];
        for (int t = 0; t < WINDOW_SIZE; t++) x[t] = window[t][ch];

        float mean  = compute_mean(x, WINDOW_SIZE);
        float std   = compute_std(x, WINDOW_SIZE, mean);
        float rms   = 0.0f; /* computed below */

        /* RMS */
        float sum_sq = 0.0f;
        for (int t = 0; t < WINDOW_SIZE; t++) sum_sq += x[t]*x[t];
        rms = sqrtf(sum_sq / WINDOW_SIZE);

        float peak   = compute_peak(x, WINDOW_SIZE);
        float kurt   = compute_kurtosis(x, WINDOW_SIZE, mean, std);
        float skew   = compute_skewness(x, WINDOW_SIZE, mean, std);
        float crest  = (rms > 1e-9f) ? peak / rms : 0.0f;

        feature_vector[feat_idx++] = mean;
        feature_vector[feat_idx++] = std;
        feature_vector[feat_idx++] = rms;
        feature_vector[feat_idx++] = peak;
        feature_vector[feat_idx++] = kurt;
        feature_vector[feat_idx++] = skew;
        feature_vector[feat_idx++] = crest;
    }

    /* ── Cross-modal decoupling features ──────────────────── */
    float I_mean = feature_vector[0];  /* CH_I_SENSOR mean     */
    float w_mean = feature_vector[7];  /* CH_OMEGA mean        */
    float Q_mean = feature_vector[14]; /* CH_Q_SENSOR mean     */
    float H_mean = feature_vector[21]; /* CH_H_SENSOR mean     */

    if (Q_mean < 1e-4f) Q_mean = 1e-4f;
    if (H_mean < 0.1f)  H_mean = 0.1f;
    if (w_mean < 1.0f)  w_mean = 1.0f;

    feature_vector[feat_idx++] = I_mean / Q_mean;               /* decoupling_IQ    */
    feature_vector[feat_idx++] = I_mean / H_mean;               /* decoupling_IH    */
    feature_vector[feat_idx++] = (Q_mean * H_mean) / (w_mean * w_mean); /* pump_eff_proxy  */
    feature_vector[feat_idx++] = w_mean / I_mean;               /* speed_load_ratio */

    return feature_vector;
}

/*
 * normalize_features()
 * Applies StandardScaler: (x - mean) / std
 * Must be called on feature_vector before predict().
 */

static void normalize_features(float* fv, int n) {
    for (int i = 0; i < n; i++) {
        fv[i] = (fv[i] - FEATURE_MEANS[i]) / (FEATURE_STDS[i] + 1e-9f);
    }
}

#endif /* ESP_FEATURES_H */
