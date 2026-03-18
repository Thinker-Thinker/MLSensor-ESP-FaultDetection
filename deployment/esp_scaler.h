#ifndef ESP_SCALER_H
#define ESP_SCALER_H

#define N_FEATURES 46

static const float FEATURE_MEANS[] = {
  14.66494566f,  /* I_sensor_mean */
  0.14430389f,  /* I_sensor_std */
  14.66567494f,  /* I_sensor_rms */
  14.94381383f,  /* I_sensor_peak */
  -0.29660066f,  /* I_sensor_kurt */
  -0.00437510f,  /* I_sensor_skew */
  1.01896633f,  /* I_sensor_crest */
  183.91981519f,  /* omega_sensor_mean */
  0.96332893f,  /* omega_sensor_std */
  183.92240315f,  /* omega_sensor_rms */
  185.80129116f,  /* omega_sensor_peak */
  -0.27793127f,  /* omega_sensor_kurt */
  0.00570432f,  /* omega_sensor_skew */
  1.01021620f,  /* omega_sensor_crest */
  0.00977169f,  /* Q_sensor_mean */
  0.00019202f,  /* Q_sensor_std */
  0.00977364f,  /* Q_sensor_rms */
  0.01014399f,  /* Q_sensor_peak */
  -0.29307040f,  /* Q_sensor_kurt */
  -0.00083145f,  /* Q_sensor_skew */
  1.03810812f,  /* Q_sensor_crest */
  39.99929304f,  /* H_sensor_mean */
  0.31210806f,  /* H_sensor_std */
  40.00054368f,  /* H_sensor_rms */
  40.59974545f,  /* H_sensor_peak */
  -0.30890766f,  /* H_sensor_kurt */
  -0.01420220f,  /* H_sensor_skew */
  1.01498003f,  /* H_sensor_crest */
  0.41504928f,  /* vib_sensor_mean */
  0.04829367f,  /* vib_sensor_std */
  0.42194185f,  /* vib_sensor_rms */
  0.50875433f,  /* vib_sensor_peak */
  -0.27449015f,  /* vib_sensor_kurt */
  0.00406808f,  /* vib_sensor_skew */
  1.41822991f,  /* vib_sensor_crest */
  0.02702833f,  /* slip_sensor_mean */
  0.00096848f,  /* slip_sensor_std */
  0.02704978f,  /* slip_sensor_rms */
  0.02889523f,  /* slip_sensor_peak */
  -0.27621439f,  /* slip_sensor_kurt */
  -0.00484882f,  /* slip_sensor_skew */
  1.08050807f,  /* slip_sensor_crest */
  1510.53426291f,  /* decoupling_IQ */
  0.36663092f,  /* decoupling_IH */
  0.00001156f,  /* pump_eff_proxy */
  12.54365093f  /* speed_load_ratio */
};

static const float FEATURE_STDS[] = {
  0.14755701f,  /* I_sensor_mean */
  0.02381190f,  /* I_sensor_std */
  0.14755814f,  /* I_sensor_rms */
  0.16440263f,  /* I_sensor_peak */
  0.75227958f,  /* I_sensor_kurt */
  0.46935758f,  /* I_sensor_skew */
  0.00475636f,  /* I_sensor_crest */
  1.66493237f,  /* omega_sensor_mean */
  0.15469932f,  /* omega_sensor_std */
  1.66492436f,  /* omega_sensor_rms */
  1.73847322f,  /* omega_sensor_peak */
  0.77943271f,  /* omega_sensor_kurt */
  0.46737083f,  /* omega_sensor_skew */
  0.00261185f,  /* omega_sensor_crest */
  0.00073673f,  /* Q_sensor_mean */
  0.00003197f,  /* Q_sensor_std */
  0.00073657f,  /* Q_sensor_rms */
  0.00074475f,  /* Q_sensor_peak */
  0.75806518f,  /* Q_sensor_kurt */
  0.47222618f,  /* Q_sensor_skew */
  0.00997534f,  /* Q_sensor_crest */
  0.06697124f,  /* H_sensor_mean */
  0.05134802f,  /* H_sensor_std */
  0.06694603f,  /* H_sensor_rms */
  0.16368322f,  /* H_sensor_peak */
  0.74029943f,  /* H_sensor_kurt */
  0.46517806f,  /* H_sensor_skew */
  0.00377373f,  /* H_sensor_crest */
  0.30814442f,  /* vib_sensor_mean */
  0.00973359f,  /* vib_sensor_std */
  0.30267306f,  /* vib_sensor_rms */
  0.30927901f,  /* vib_sensor_peak */
  0.77314278f,  /* vib_sensor_kurt */
  0.48665831f,  /* vib_sensor_skew */
  0.39108477f,  /* vib_sensor_crest */
  0.00877315f,  /* slip_sensor_mean */
  0.00020188f,  /* slip_sensor_std */
  0.00876282f,  /* slip_sensor_rms */
  0.00877762f,  /* slip_sensor_peak */
  0.76929940f,  /* slip_sensor_kurt */
  0.48440842f,  /* slip_sensor_skew */
  0.04968922f,  /* slip_sensor_crest */
  129.98532977f,  /* decoupling_IQ */
  0.00371773f,  /* decoupling_IH */
  0.00000085f,  /* pump_eff_proxy */
  0.22958030f  /* speed_load_ratio */
};

#endif /* ESP_SCALER_H */
