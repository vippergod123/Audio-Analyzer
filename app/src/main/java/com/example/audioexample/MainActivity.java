package com.example.audioexample;

import androidx.appcompat.app.AppCompatActivity;

import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.view.View;

import com.example.audioexample.utils.Constant;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {

  // Used to load the 'native-lib' library on application startup.
  static {
    System.loadLibrary("native-lib");
  }

  private static final int FILE_SELECT_CODE = 0x0001;
  public static final String TAG = "DUY_TAG";
  MediaPlayer mediaPlayer;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

  }

  public void onClickPlayAudio(View view) {
    handleAudioFile();
  }

  private void handleAudioFile() {
    try {
      mediaPlayer = new MediaPlayer();
      mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
      mediaPlayer.setDataSource(Constant.waveSoldOutUrl);
      mediaPlayer.prepareAsync();
      mediaPlayer.setOnPreparedListener(new MediaPlayer.OnPreparedListener(){

        @Override
        public void onPrepared(MediaPlayer mediaPlayer) {
          mediaPlayer.start();
        }
      });

    } catch (IOException e) {
      e.printStackTrace();
    }
  }

  /**
   * A native method that is implemented by the 'native-lib' native library,
   * which is packaged with this application.
   */
  public native String stringFromJNI();

  public native String audioFileTest();


}

//  void testFFTLib() {
//    FFT kissFastFourierTransformer = new FFT();
//    FastFourierTransformer fastFourierTransformer = new FastFourierTransformer(DftNormalization.STANDARD);
//
//    final int testsize = 64;
//    Complex[] indata = new Complex[testsize];
//    for (int i = 0; i < testsize; i++)
//      indata[i] = new Complex(Math.random() - 0.5);
//    Complex[] outdata1 = kissFastFourierTransformer.transform(indata, TransformType.FORWARD);
//    Complex[] outdata2 = fastFourierTransformer.transform(indata, TransformType.FORWARD);
//    for (int i = 0; i < testsize; i++) {
//      double err = outdata1[i].subtract(outdata2[i]).abs();
//      Log.d("FFTTest", "" + i +
//            " (" + outdata1[i].getReal() + "," + outdata1[i].getImaginary() + ") -- " +
//            " (" + outdata2[i].getReal() + "," + outdata2[i].getImaginary() + ") = " + err);
//    }
//  }

