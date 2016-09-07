package autoupdate;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

import org.xmlpull.v1.XmlPullParser;

import android.app.ProgressDialog;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.util.Xml;

public class DataTransfer {
/* GetFileFromUrl
 * ParserXmlData
 * */
		
	private static void GetFileAndShowProcessFromUrl(String urlPath, String filePath, ProgressDialog pd) throws Exception
	{
		URL url = null;
		HttpURLConnection conn = null;
		InputStream is = null;
		File file = null;
		FileOutputStream fos = null;
		BufferedInputStream bis = null;
		int len = 0;
		int total = 0;
		byte[] buffer = new byte[1024];
		
		//input buffer form remote file	
		Log.i(Common.TAG,"urlPath is "+urlPath+ "filePath is "+ filePath);
		url = new URL(urlPath);
		conn =  (HttpURLConnection) url.openConnection();
		
		if( null == conn )
		{
			Log.i(Common.TAG, "conn is null");
		}
		
		conn.setConnectTimeout(5000);		
		

		is = conn.getInputStream();
		bis = new BufferedInputStream(is);
			
		//output to local file
		file = new File(filePath);
		fos = new FileOutputStream(file);        
		
		//transfer
		if( null != pd)
		{
			pd.setMax(conn.getContentLength());
		}
		
		while((len =bis.read(buffer))!=-1){
			fos.write(buffer, 0, len);
			total+= len;
			if( null != pd )
			{
				pd.setProgress(total);
			}
		}

		//close
		fos.close();
		bis.close();
		is.close();	
	}
	
	private static UpdateInfo getUpdateInfoFromRemoteXmlFile(String urlPath, String filePath) throws Exception{
		
		DataTransfer.GetFileAndShowProcessFromUrl(urlPath, filePath, null);	
		File file = new File(filePath);
		InputStream in = new FileInputStream(file);		
		return getUpdateInfo(in);		
	}
	/*
	 * 用pull解析器解析服务器返回的xml文件 (xml封装了版本号)
	 */
	private static UpdateInfo getUpdateInfo(InputStream is) throws Exception{
		XmlPullParser  parser = Xml.newPullParser();  
		parser.setInput(is, "utf-8");//设置解析的数据源 
		int type = parser.getEventType();
		UpdateInfo info = new UpdateInfo();//实体
		while(type != XmlPullParser.END_DOCUMENT ){
			switch (type) {
			case XmlPullParser.START_TAG:
				if("version".equals(parser.getName())){
					info.setVersion(parser.nextText());	//获取版本号
				}else if ("url".equals(parser.getName())){
					info.setUrl(parser.nextText());	//获取要升级的APK文件
				}else if ("description".equals(parser.getName())){
					info.setDescription(parser.nextText());	//获取该文件的信息
				}else if ("type".equals(parser.getName())){
					info.setType(parser.nextText());	//获取该文件的信息
				}
				break;
			}
			type = parser.next();
		}
		info.dump();
		return info;
	}
	
	/*
	 * 从服务器中下载APK
	 */
	public static void downLoadApkOrPatch(final String url, final String filePath, Context context, final Handler handler) {
		final ProgressDialog pd;	//进度条对话框
		pd = new  ProgressDialog(context);
		pd.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
		pd.setMessage("正在下载更新");
		pd.show();
		Log.i(Common.TAG, "downloadApkOrPatch==========");
		new Thread(){
			@Override
			public void run() {
				try {
					DataTransfer.GetFileAndShowProcessFromUrl(url, filePath, pd);
					Log.i(Common.TAG,"Finish downloading patch file");
					pd.dismiss(); //结束掉进度条对话框
					
					Message msg = new Message();
					msg.what = Common.DOWNLOAD_APK_OK;
					handler.sendMessage(msg);
					
				} catch (Exception e) {
					Message msg = new Message();
					msg.what = Common.DOWNLOAD_APK_ERROR;
					handler.sendMessage(msg);
					e.printStackTrace();
				}
			}}.start();
	}
	
	/*
	 * 从服务器中下载update.xml
	 */
	public static void downLoadUpdateXml(final String url, final Handler handler) {
		new Thread(){
			@Override
			public void run() {
				try {
					UpdateInfo updateInfo = DataTransfer.getUpdateInfoFromRemoteXmlFile(url,Common.xmlPath);
					updateInfo.dump();
					
					Message msg = new Message();
					Bundle bundle = new Bundle();
					bundle.putString("description", updateInfo.getDescription());
					bundle.putString("url", updateInfo.getUrl());
					bundle.putString("type", updateInfo.getType());
					bundle.putString("version", updateInfo.getVersion());
					msg.setData(bundle);
					msg.what = Common.DOWNLOAD_XML_OK;
					handler.sendMessage(msg);
					
				} catch (Exception e) {
					Message msg = new Message();
					msg.what = Common.DOWNLOAD_XML_ERROR;
					handler.sendMessage(msg);
					e.printStackTrace();
				}
			}}.start();
	}
}
