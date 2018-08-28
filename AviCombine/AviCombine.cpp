#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
typedef long DWORD;
#endif

extern "C" {
#include "avilib.h"
}
/*
合并2个avi
by 沈亚军
使用avilib进行avi文件的读写 
这个例子是用来将两个avi合并成为一个
just a simple test 
*/



int combine_avi_file(char * src1_file,char * src2_file,char * dst_file)
{

   // 分配音视频数据缓存
    char * vidbuf = (char *)malloc(608256);
    char * audbuf = (char *)malloc(100000);
    long framesize;
    avi_t *avifile1=0;
    avi_t *avifile2=0;
    avi_t* pAviHandle=0;
    int frame;
    int frames1;
    int frames2;
    int framew = 0;
    int frameh = 0;
    double framerate = 0.0f;

   // 打开目标文件
    pAviHandle = AVI_open_output_file(dst_file);    
    if(!pAviHandle)
    {     
		free(vidbuf);
		free(audbuf);
        return -1; 
    }
	//打开源文件1
    avifile1 = AVI_open_input_file(src1_file, 1);  
	if (!avifile1)
	{
		AVI_close(pAviHandle);    
		free(vidbuf);
		free(audbuf);
		return -2; 
	}
	//打开源文件2 
    avifile2 = AVI_open_input_file(src2_file, 1); 
	if (!avifile2)
	{
		AVI_close(pAviHandle);    
		free(vidbuf);
		free(audbuf);
		return -2; 
	}
    // 获取源视频信息
    frames1 = AVI_video_frames(avifile1);
	frames2=AVI_video_frames(avifile2);
	
    framew = AVI_video_width(avifile1);
    frameh = AVI_video_height(avifile1);
    framerate = AVI_frame_rate(avifile1);    
    //设置video文件头 
      AVI_set_video(pAviHandle,framew,frameh,framerate,AVI_video_compressor(avifile1)); 
	     // 设置audio文件头 
      AVI_set_audio(pAviHandle, AVI_audio_channels(avifile1)+AVI_audio_channels(avifile2), 
                                AVI_audio_rate(avifile1), 
                                AVI_audio_bits(avifile1)+AVI_audio_bits(avifile2),
                                AVI_audio_format(avifile1),
                                AVI_audio_mp3rate(avifile1));
                            
    framesize=(long)framew*frameh*3/2;
	 //寻找文件1开始 
      if (AVI_seek_start( avifile1 ))  
      {
             printf("bad seek start/n");             
             return -2; 
      }

	  for (frame = 0; frame < frames1;frame++)//read and write avi per fream 
	  {
		  int iskeyframe;//if it's keyframe
		  AVI_set_video_position(avifile1, frame);//将avi文件移动到第fream帧
		  long audio_bytes=AVI_audio_size(avifile1,frame);//get audo size 

		  AVI_read_frame(avifile1,(char *)vidbuf,&iskeyframe);//read fream set video into vidbuf 
		  AVI_read_audio(avifile1, (char *)audbuf,audio_bytes);//set audio into audbuf
		  /*
		  long AVI_audio_size(avi_t *AVI, long frame)
		  int AVI_write_audio(avi_t *AVI, char *data, long bytes)
		  long AVI_read_audio(avi_t *AVI, char *audbuf, long bytes)
		  */
		  AVI_write_frame(pAviHandle,vidbuf,AVI_frame_size(avifile1, frame),iskeyframe);//write video fream by fream
		  AVI_write_audio(pAviHandle,audbuf,audio_bytes);//write audio fream by fream
	  } 
      AVI_close(avifile1);

//寻找文件2开始
	  if (AVI_seek_start( avifile2 )) 
	  {
		  printf("bad seek start/n");             
		  return -3; 
	  }
	  for (frame = 0; frame < frames2;frame++)//read and write avi per fream 
	  {
		  int iskeyframe;//if it's keyframe
		  AVI_set_video_position(avifile2, frame);//将avi文件移动到第fream帧
		  long audio_bytes=AVI_audio_size(avifile2,frame);//get audo size 

		  AVI_read_frame(avifile2,(char *)vidbuf,&iskeyframe);//read fream set video into vidbuf 
		  AVI_read_audio(avifile2, (char *)audbuf,audio_bytes);//set audio into audbuf
		  /*
		  long AVI_audio_size(avi_t *AVI, long frame)
		  int AVI_write_audio(avi_t *AVI, char *data, long bytes)
		  long AVI_read_audio(avi_t *AVI, char *audbuf, long bytes)
		  */
		  AVI_write_frame(pAviHandle,vidbuf,AVI_frame_size(avifile2,frame),iskeyframe);//write video fream by fream
		  AVI_write_audio(pAviHandle,audbuf,audio_bytes);//write audio fream by fream
	  } 

	    
	
      AVI_close(avifile2);
	   AVI_close(pAviHandle);
	  return 1; 
}
int main(int argc, const char **argv)
{
	combine_avi_file("原始1.avi","原始2.avi","目标.avi");
	return 1; 
}