package com.example.audioexample;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.Intent;
import android.graphics.Color;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaMetadataRetriever;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.text.Html;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.recyclerview.widget.DefaultItemAnimator;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.example.audioexample.recyclerview.MusicRecyclerViewAdapter;
import com.example.audioexample.utils.Constant;
import com.example.audioexample.utils.ZAsyncTask;
import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.Description;
import com.github.mikephil.charting.components.LimitLine;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, MusicRecyclerViewAdapter.MusicRecyclerViewListener, AdapterView.OnItemSelectedListener {


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
   RecyclerView recyclerView;
   List<String> musicList = new ArrayList<String>();
   MusicRecyclerViewAdapter adapter;
   Button playButton;
   SeekBar seekBar;
   TextView durationTextView;
   TextView nowPlayingTextView;


   LineChart lineChart;
   Spinner dropDownSpinner;
   List<Integer> dropDownItems = new ArrayList<>(Arrays.asList(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
   int SPLIT_BUFFER_BY_SECOND = 5;

   @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
   @Override
   protected void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
      setContentView(R.layout.activity_main);
      ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 0);
      init();
      setOnClickListenerForView();
   }

   @Override
   protected void onResume() {
      super.onResume();
      prepareData();
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
         case R.id.load_file_button:
            loadFileAudio();
            break;
         case R.id.custom_sound_button:
            onClickCustomSound();
            break;
         case R.id.read_buffer_stream_button:
            onClickReadBuffer();
         case R.id.play_button:
            playSound();
            break;
      }
   }

   @Override
   public void onClickViewHolder(String musicName) {
      selectAudio(musicName);
   }


   @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
   private void init() {
      recyclerView = (RecyclerView) findViewById(R.id.recyclerView);
      playButton = (Button) findViewById(R.id.play_button);
      seekBar = (SeekBar) findViewById(R.id.seek_bar);
      durationTextView = (TextView) findViewById(R.id.duration_text_view);
      nowPlayingTextView = (TextView) findViewById(R.id.nowPlayingTextView);
      lineChart = (LineChart) findViewById(R.id.bar_chart);

      dropDownSpinner = findViewById(R.id.drop_down_list);
      dropDownSpinner.setAdapter(new ArrayAdapter<>(this, android.R.layout.simple_spinner_dropdown_item, dropDownItems));
      dropDownSpinner.setSelection(dropDownItems.indexOf(SPLIT_BUFFER_BY_SECOND));

      adapter = new MusicRecyclerViewAdapter(this, musicList);
      recyclerView.setAdapter(adapter);

      RecyclerView.LayoutManager mLayoutManager = new LinearLayoutManager(getApplicationContext());
      recyclerView.setLayoutManager(mLayoutManager);

      recyclerView.setItemAnimator(new DefaultItemAnimator());


   }

   private void setOnClickListenerForView() {
      findViewById(R.id.load_file_button).setOnClickListener(this);
      findViewById(R.id.read_buffer_stream_button).setOnClickListener(this);
      findViewById(R.id.custom_sound_button).setOnClickListener(this);
      findViewById(R.id.play_button).setOnClickListener(this);

      adapter.setOnClickViewHolderListener(this);

      dropDownSpinner.setOnItemSelectedListener(this);

      seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
         @Override
         public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
            Log.d(TAG, "onProgressChanged");
         }

         @Override
         public void onStartTrackingTouch(SeekBar seekBar) {
            Log.d(TAG, "onStartTrackingTouch");
         }

         @Override
         public void onStopTrackingTouch(SeekBar seekBar) {
            if (mediaPlayer != null && mediaPlayer.isPlaying()) {
               mediaPlayer.seekTo(seekBar.getProgress() * 1000);
               handler.removeCallbacksAndMessages(null);
               updateSeekBarAndMinuteTextView(0);
            }
         }
      });

   }

   private void prepareData() {
      File directory = new File(downloadPath);
      File[] files = directory.listFiles();
      if (files != null) {
         for (File file : files) {
            String nameFile = file.getName();
            if (nameFile.endsWith("wav") || nameFile.endsWith("mp3")) {
               musicList.add(nameFile);
            }
         }
      }
   }

   private void playSound() {
      try {
         if (mediaPlayer == null || !mediaPlayer.isPlaying()) {
            mediaPlayer = new MediaPlayer();
            mediaPlayer.setDataSource(musicPath);
            mediaPlayer.prepare();
            mediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
               @Override
               public void onCompletion(MediaPlayer mediaPlayer) {
                  playButton.setText("Play Sound");
               }
            });
            seekBar.setMax(mediaPlayer.getDuration() / 1000);
            playButton.setText("Stop Sound");


            mediaPlayer.start();
            updateSeekBarAndMinuteTextView(0);
         } else {
            playButton.setText("Play Sound");
            mediaPlayer.stop();
            mediaPlayer.release();
            mediaPlayer = null;
         }

      } catch (IOException e) {
         e.printStackTrace();
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

      try {
         music = new byte[4096];
         at.play();

         while ((i = is.read(music)) != -1)
            at.write(music, 0, i);

      } catch (IOException e) {
         e.printStackTrace();
      }

      at.stop();
      at.release();
   }

   private void loadFileAudio() {
      final Runnable r = new Runnable() {
         public void run() {
            double[] arrayPcmValue = getPcmAudioValue(musicPath, SPLIT_BUFFER_BY_SECOND);
            int index = 0;
            for (double each :
                 arrayPcmValue) {
               Log.d(TAG, index++ + ": " + each);
            }

            renderBarChart(arrayPcmValue);
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

   @SuppressLint("SetTextI18n")
   private void selectAudio(String fileName) {
      musicPath = downloadPath + "/" + fileName;
      MediaMetadataRetriever mmr = new MediaMetadataRetriever();
      mmr.setDataSource(musicPath);
      String durationStr = mmr.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION);
      int millSecond = Integer.parseInt(durationStr) / 1000;
      String sourceString = "File selected: " + "<b>" + fileName + "</b> - Duration: " + millSecond;
      nowPlayingTextView.setText(Html.fromHtml(sourceString));
   }

   private void updateSeekBarAndMinuteTextView(double delay) {

      handler.postDelayed(new Runnable() {
         @Override
         public void run() {
            if (mediaPlayer != null && mediaPlayer.isPlaying()) {
               float duration = mediaPlayer.getCurrentPosition() / 1000f;
               seekBar.setProgress((int) duration);
               durationTextView.setText(String.valueOf((int) duration));

               if (lineChart != null) {
                  XAxis xAxis = lineChart.getXAxis();
                  xAxis.removeAllLimitLines();

                  LimitLine ll = new LimitLine(duration, (int) duration + "");
                  ll.setLineColor(Color.RED);
                  ll.setLineWidth(1f);
                  ll.setTextColor(Color.DKGRAY);
                  ll.setTextSize(12f);
                  xAxis.addLimitLine(ll);
                  lineChart.invalidate();
               }
               updateSeekBarAndMinuteTextView(0.1);
            } else {
               if (lineChart != null) {
                  lineChart.getXAxis().removeAllLimitLines();
               }
               durationTextView.setText("00");
               seekBar.setProgress(0);
            }
         }
      }, (int) delay * 1000);
   }

   private void renderBarChart(double[] arrayPcmValue) {

      LineDataSet lineDataSet = new LineDataSet(createLineDataEntries(arrayPcmValue), "asdjosajdo");
      LineData lineData = new LineData(lineDataSet);
      lineChart.setData(lineData);
      Description lineChartDescription = new Description();
      lineChart.setDescription(lineChartDescription);

      lineChart.setDrawGridBackground(false);
      lineChart.setDrawBorders(true);
      lineChart.getAxisLeft().setAxisMinimum(0);


      lineDataSet.setColors(Color.BLACK);
      lineDataSet.setValueTextColor(Color.BLACK);
      lineDataSet.setValueTextSize(10f);

      lineChart.invalidate();
   }

   private ArrayList createLineDataEntries(double[] arrayPcmValue) {
      ArrayList lineEntries = new ArrayList<>();
      if (arrayPcmValue != null) {
         int index = 0;
         for (double value : arrayPcmValue) {
            lineEntries.add(new Entry(index++ * SPLIT_BUFFER_BY_SECOND, (float) value));
         }
      }

      return lineEntries;

   }


   public native String stringFromJNI();

   public native String audioFileTest(String filePath);

   public native String convertByteArrayToUnsignChar(byte[] dataArray);

   public native double[] getPcmAudioValue(String filePath, int splitBy);

   @Override
   public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
      SPLIT_BUFFER_BY_SECOND = dropDownItems.get(i);
   }

   @Override
   public void onNothingSelected(AdapterView<?> adapterView) {

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

