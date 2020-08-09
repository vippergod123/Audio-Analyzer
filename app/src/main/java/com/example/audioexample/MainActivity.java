package com.example.audioexample;

import android.Manifest;
import android.content.Intent;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import com.example.audioexample.utils.Constant;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {


   // Used to load the 'native-lib' library on application startup.
   static {
      System.loadLibrary("native-lib");
   }

   private static final int AUDIO_FILE_REQUEST_CODE = 0x0001;
   public static final String TAG = "JAVA_CODE";
   MediaPlayer mediaPlayer;

   @Override
   protected void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
      setContentView(R.layout.activity_main);
      ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 0);
   }

   @Override
   protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
      super.onActivityResult(requestCode, resultCode, data);

      if (requestCode == 1) {
         if (resultCode == RESULT_OK) {
            //the selected audio.
            Uri uri = data.getData();
            String path = uri.getPath();
            audioFileTest(path);
         }
      }
   }

   public void onClickPlayAudio(View view) {
//      startChooseAudioFile();


      Handler handler = new Handler();

      final Runnable r = new Runnable() {
         public void run() {
            String path = Environment.getExternalStorageDirectory().getPath();
            String filePath = path + "/Download/silent.wav";
            audioFileTest(filePath);
         }
      };

      handler.post(r);
   }

   private void handleAudioFile() {
      try {
         mediaPlayer = new MediaPlayer();
         mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
         mediaPlayer.setDataSource(Constant.waveSoldOutUrl);
         mediaPlayer.prepareAsync();
         mediaPlayer.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {

            @Override
            public void onPrepared(MediaPlayer mediaPlayer) {
               mediaPlayer.start();
            }
         });

      } catch (IOException e) {
         e.printStackTrace();
      }
   }

   private void handleAudioFileInStorage() {
      Log.e("DUY", Environment.getExternalStorageDirectory().getPath());
//    MediaPlayer mp = MediaPlayer.create(this, Uri.parse(Environment.getExternalStorageDirectory().getPath()+ "/Download/wave.wav"));
//    mp.start();
   }

   public void startChooseAudioFile() {
      Intent intent_upload = new Intent();
      intent_upload.setType("audio/*");
      intent_upload.setAction(Intent.ACTION_GET_CONTENT);
      startActivityForResult(intent_upload, AUDIO_FILE_REQUEST_CODE);
   }

   /**
    * A native method that is implemented by the 'native-lib' native library,
    * which is packaged with this application.
    */
   public native String stringFromJNI();

   public native String audioFileTest(String filePath);


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

