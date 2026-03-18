#ifndef ESP_SCALER_H
#define ESP_SCALER_H

#define N_FEATURES 46

static const float FEATURE_MEANS[] = {
  14.65316369f,  /* I_sensor_mean */
  0.14433993f,  /* I_sensor_std */
  14.65389364f,  /* I_sensor_rms */
  14.93274441f,  /* I_sensor_peak */
  -0.29586096f,  /* I_sensor_kurt */
  0.00062283f,  /* I_sensor_skew */
  1.01903227f,  /* I_sensor_crest */
  184.01657314f,  /* omega_sensor_mean */
  0.96393570f,  /* omega_sensor_std */
  184.01916383f,  /* omega_sensor_rms */
  185.89094734f,  /* omega_sensor_peak */
  -0.28592880f,  /* omega_sensor_kurt */
  -0.00034631f,  /* omega_sensor_skew */
  1.01017241f,  /* omega_sensor_crest */
  0.00970573f,  /* Q_sensor_mean */
  0.00019252f,  /* Q_sensor_std */
  0.00970771f,  /* Q_sensor_rms */
  0.01007998f,  /* Q_sensor_peak */
  -0.27132694f,  /* Q_sensor_kurt */
  -0.00462787f,  /* Q_sensor_skew */
  1.03864329f,  /* Q_sensor_crest */
  40.00071097f,  /* H_sensor_mean */
  0.31103379f,  /* H_sensor_std */
  40.00195210f,  /* H_sensor_rms */
  40.60337535f,  /* H_sensor_peak */
  -0.32817534f,  /* H_sensor_kurt */
  0.00852557f,  /* H_sensor_skew */
  1.01503512f,  /* H_sensor_crest */
  0.49032820f,  /* vib_sensor_mean */
  0.04840083f,  /* vib_sensor_std */
  0.49484253f,  /* vib_sensor_rms */
  0.58366030f,  /* vib_sensor_peak */
  -0.29622125f,  /* vib_sensor_kurt */
  -0.01334176f,  /* vib_sensor_skew */
  1.29337894f,  /* vib_sensor_crest */
  0.02650964f,  /* slip_sensor_mean */
  0.00096716f,  /* slip_sensor_std */
  0.02653196f,  /* slip_sensor_rms */
  0.02838616f,  /* slip_sensor_peak */
  -0.27143386f,  /* slip_sensor_kurt */
  0.00342368f,  /* slip_sensor_skew */
  1.08455147f,  /* slip_sensor_crest */
  1521.18038921f,  /* decoupling_IQ */
  0.36632370f,  /* decoupling_IH */
  0.00001147f,  /* pump_eff_proxy */
  12.56068987f  /* speed_load_ratio */
};

static const float FEATURE_STDS[] = {
  0.15797080f,  /* I_sensor_mean */
  0.02353043f,  /* I_sensor_std */
  0.15795494f,  /* I_sensor_rms */
  0.17182928f,  /* I_sensor_peak */
  0.76842595f,  /* I_sensor_kurt */
  0.46675513f,  /* I_sensor_skew */
  0.00480581f,  /* I_sensor_crest */
  1.81583154f,  /* omega_sensor_mean */
  0.15564961f,  /* omega_sensor_std */
  1.81581323f,  /* omega_sensor_rms */
  1.87971635f,  /* omega_sensor_peak */
  0.75396735f,  /* omega_sensor_kurt */
  0.46599590f,  /* omega_sensor_skew */
  0.00254004f,  /* omega_sensor_crest */
  0.00079277f,  /* Q_sensor_mean */
  0.00003192f,  /* Q_sensor_std */
  0.00079258f,  /* Q_sensor_rms */
  0.00079635f,  /* Q_sensor_peak */
  0.77445043f,  /* Q_sensor_kurt */
  0.47659022f,  /* Q_sensor_skew */
  0.01060128f,  /* Q_sensor_crest */
  0.06662063f,  /* H_sensor_mean */
  0.05050503f,  /* H_sensor_std */
  0.06661229f,  /* H_sensor_rms */
  0.16012565f,  /* H_sensor_peak */
  0.73078666f,  /* H_sensor_kurt */
  0.46078799f,  /* H_sensor_skew */
  0.00370380f,  /* H_sensor_crest */
  0.28708575f,  /* vib_sensor_mean */
  0.01021862f,  /* vib_sensor_std */
  0.28358051f,  /* vib_sensor_rms */
  0.28796160f,  /* vib_sensor_peak */
  0.76872834f,  /* vib_sensor_kurt */
  0.46937237f,  /* vib_sensor_skew */
  0.28531299f,  /* vib_sensor_crest */
  0.00956419f,  /* slip_sensor_mean */
  0.00020033f,  /* slip_sensor_std */
  0.00955329f,  /* slip_sensor_rms */
  0.00957182f,  /* slip_sensor_peak */
  0.78966493f,  /* slip_sensor_kurt */
  0.48466944f,  /* slip_sensor_skew */
  0.05280317f,  /* slip_sensor_crest */
  140.33972098f,  /* decoupling_IQ */
  0.00400468f,  /* decoupling_IH */
  0.00000091f,  /* pump_eff_proxy */
  0.24842989f  /* speed_load_ratio */
};

#endif /* ESP_SCALER_H */
