package com.example.audioexample.utils;

public interface IDownloadListener {

  /**
   * Notifies about download completion.
   *
   * @param buf
   * @param offset
   * @param length
   */
  public void onComplete(final byte[] buf, final int offset, final int length);
}
