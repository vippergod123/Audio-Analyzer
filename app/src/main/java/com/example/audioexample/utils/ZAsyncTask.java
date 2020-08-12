package com.example.audioexample.utils;

import android.os.AsyncTask;

public class ZAsyncTask extends AsyncTask<Runnable, Void, String> {

  @Override
  protected String doInBackground(Runnable... runnables) {

    for (Runnable each : runnables) {
      each.run();
    }
    return null;
  }

  protected void onPostExecute(String feed) {
    // TODO: check this.exception
    // TODO: do something with the feed
  }
}