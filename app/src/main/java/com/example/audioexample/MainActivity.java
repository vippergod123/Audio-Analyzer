package com.example.audioexample;

import android.Manifest;
import android.content.Intent;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import com.example.audioexample.utils.Constant;
import com.example.audioexample.utils.ZAsyncTask;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Random;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {


   // Used to load the 'native-lib' library on application startup.
   static {
      System.loadLibrary("native-lib");
   }

   private static final int AUDIO_FILE_REQUEST_CODE = 0x0001;
   public static final String TAG = "JAVA_CODE";
   MediaPlayer mediaPlayer;
   Handler handler = new Handler();
   String downloadPath = Environment.getExternalStorageDirectory().getPath() + "/Download";
   String musicPath = downloadPath + "/" + Constant.localWaveLowFile;

   @Override
   protected void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
      setContentView(R.layout.activity_main);
      ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 0);
      setOnClickListenerForView();
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

   @Override
   public void onClick(View view) {
      int id = view.getId();
      switch (id) {

         case R.id.silent_wav_text_view:
            selectAudio(Constant.localWaveSilentFile);
            break;
         case R.id.wav_text_view:
            selectAudio(Constant.localWaveLowFile);
            break;
         case R.id.music_wav_text_view:
            selectAudio(Constant.localMusicFile);
            break;
         case R.id.load_file_button:
            onClickPlayAudio();
            break;
         case R.id.custom_sound_button:
            onClickCustomSound();
            break;
         case R.id.read_buffer_stream_button:
            onClickReadBuffer();
            break;
      }
   }



   private void onClickCustomSound() {
      int i = 0;
      byte[] music = null;
      InputStream is = getResources().openRawResource(R.raw.silent);

      int bufsize = AudioTrack.getMinBufferSize(
            4096,
            AudioFormat.CHANNEL_OUT_STEREO,
            AudioFormat.ENCODING_PCM_16BIT
      );

      AudioTrack at = new AudioTrack(AudioManager.STREAM_MUSIC, 44100,
            AudioFormat.CHANNEL_OUT_STEREO, AudioFormat.ENCODING_PCM_16BIT,
            bufsize, AudioTrack.MODE_STREAM);

      try{
         music = new byte[4096];
         at.play();

         while((i = is.read(music)) != -1)
            at.write(music, 0, i);

      } catch (IOException e) {
         e.printStackTrace();
      }

      at.stop();
      at.release();
   }

   private void onClickPlayAudio() {


      final Runnable r = new Runnable() {
         public void run() {
            audioFileTest(musicPath);
         }
      };

      handler.post(r);
   }

   private void onClickReadBuffer() {


      final Runnable r = new Runnable() {
         @RequiresApi(api = Build.VERSION_CODES.O)
         public void run() {
            try {
               Path fileLocation = Paths.get(musicPath);
               byte[] data = Files.readAllBytes(fileLocation);
               convertByteArrayToUnsignChar(data);
            } catch (IOException e) {
               e.printStackTrace();
            }
         }
      };
      new ZAsyncTask().execute(r);
   }

   private void selectAudio(String fileName) {
      musicPath = downloadPath + "/" + fileName;
      Toast.makeText(this, "File selected: " + fileName, Toast.LENGTH_SHORT).show();
   }

   /**
    * A native method that is implemented by the 'native-lib' native library,
    * which is packaged with this application.
    */
   public native String stringFromJNI();

   public native String audioFileTest(String filePath);

   public native String convertByteArrayToUnsignChar(byte[] dataArray);


   private void setOnClickListenerForView() {
      findViewById(R.id.silent_wav_text_view).setOnClickListener(this);
      findViewById(R.id.wav_text_view).setOnClickListener(this);
      findViewById(R.id.music_wav_text_view).setOnClickListener(this);

      findViewById(R.id.load_file_button).setOnClickListener(this);
      findViewById(R.id.read_buffer_stream_button).setOnClickListener(this);
      findViewById(R.id.custom_sound_button).setOnClickListener(this);
   }
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

