package com.example.audioexample.recyclerview;

import android.content.Context;
import android.text.Layout;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.example.audioexample.R;

import java.util.List;

public class MusicRecyclerViewAdapter extends RecyclerView.Adapter<MusicRecyclerViewAdapter.MusicViewHolder> {

   public interface MusicRecyclerViewListener {
      void onClickViewHolder(String musicName);
   }

   private List<String> musicList;
   private Context context;

   private MusicRecyclerViewListener listener;

   public MusicRecyclerViewAdapter(Context c, List<String> data) {
      this.musicList = data;
      this.context = c;
   }

   @NonNull
   @Override
   public MusicViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
      View v = LayoutInflater.from(context).inflate(R.layout.music_view_holder, parent, false);
      return new MusicViewHolder(v);
   }

   @Override
   public void onBindViewHolder(@NonNull MusicViewHolder holder, int position) {
      holder.onBindViewHolder(musicList.get(position), listener);
   }


   @Override
   public int getItemCount() {
      return musicList.size();
   }

   public static class MusicViewHolder extends RecyclerView.ViewHolder {
      // each data item is just a string in this case
      public TextView musicTextView;

      public MusicViewHolder(View v) {
         super(v);
         musicTextView = v.findViewById(R.id.musicTextView);
      }

      void onBindViewHolder(final String musicName, final MusicRecyclerViewListener listener) {
         musicTextView.setText(musicName);
         musicTextView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
               listener.onClickViewHolder(musicName);
            }
         });
      }
   }

   public void setOnClickViewHolderListener(MusicRecyclerViewListener listener) {
      this.listener = listener;
   }

}
