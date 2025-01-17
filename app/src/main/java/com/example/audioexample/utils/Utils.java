package com.example.audioexample.utils;

import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.provider.DocumentsContract;
import android.provider.MediaStore;

public class Utils {
  public static String getPath(Context context,Uri uri) {
    String filePath = "";
    String wholeID = DocumentsContract.getDocumentId(uri);

    // Split at colon, use second item in the array
    String id = wholeID.split(":")[1];

    String[] column = { MediaStore.Images.Media.DATA };

    // where id is equal to
    String sel = MediaStore.Images.Media._ID + "=?";

    Cursor cursor = context.getContentResolver().query(MediaStore.Images.Media.EXTERNAL_CONTENT_URI,
          column, sel, new String[]{ id }, null);

    int columnIndex = cursor.getColumnIndex(column[0]);

    if (cursor.moveToFirst()) {
      filePath = cursor.getString(columnIndex);
    }
    cursor.close();
    return filePath;
  }
}
