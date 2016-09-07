package autoupdate;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

import com.example.autoupdatetest.MainActivity;
import com.example.autoupdatetest.R;

import autoupdate.DataTransfer;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.widget.Toast;


public class CheckVerActivity extends Activity {
	Handler handler = null;
	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//setContentView(R.layout.activity_main)

		try {
			init();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		handleMsg();
		DataTransfer.downLoadUpdateXml(Common.serverUrl,handler);
		
	}
	
	void init() throws Exception
	{
		PackageManager packageManager = this.getPackageManager();  
	    PackageInfo packInfo = packageManager.getPackageInfo(this.getPackageName(), 0);
        Common.localVersion =  packInfo.versionName;	
        Common.context = this;
        
        Common.serverUrl = this.getResources().getString(R.string.server_url);
        Common.bspatchPath = this.getResources().getString(R.string.bspatch_path);
        Common.oldVersionApk = this.getResources().getString(R.string.old_version_apk);
        Common.newVersionApk = this.getResources().getString(R.string.new_version_apk);
        Common.newVersionPatch = this.getResources().getString(R.string.new_version_patch);
        Common.xmlPath = this.getResources().getString(R.string.xml_path);
        Common.bspatchCmd = Common.bspatchPath + " " + Common.oldVersionApk + " " + Common.newVersionApk + " " + Common.newVersionPatch;  
	}
	
	void handleMsg()
	{
        handler = new Handler(){
			
			@Override
			public void handleMessage(Message msg) {
				// TODO Auto-generated method stub
				super.handleMessage(msg);
				switch (msg.what) {
				case Common.DOWNLOAD_XML_OK:
					Log.i(Common.TAG, "Receive msg: DOWNLOAD_XML_OK");
					Bundle bundle = msg.getData();
					UpdateInfo updateInfo = new UpdateInfo();
					updateInfo.setDescription(bundle.getString("description"));
					updateInfo.setUrl(bundle.getString("url"));
					updateInfo.setVersion(bundle.getString("version"));
					updateInfo.setType(bundle.getString("type"));
					updateInfo.dump();
					VersionManager.SetUpdateInfo(updateInfo);
					
					try 
					{
						if( false == VersionManager.CompareVersion())
						{
							showUpdataDialog(updateInfo.getDescription(), updateInfo.getUrl());
						}
					} catch (Exception e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
					
					break;
					
				case Common.DOWNLOAD_APK_OK:
					Log.i(Common.TAG, "Receive msg:DOWNLOAD_APK_END");
					Log.i(Common.TAG, "Start to execute bspatch Commond");
					ExecShellCmd.setPermission(Common.bspatchPath);
					try {
						ExecShellCmd.persistPatchTool( Common.context.getAssets().open("bspatch") );
						ExecShellCmd.execCmd(Common.bspatchCmd);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					Log.i(Common.TAG, "Start to install APK");
					installApk(new File(Common.newVersionApk));
					break;	
				}
			}
		};	
	}
	/*
	 * 
	 * �����Ի���֪ͨ�û����³��� 
	 * 
	 * �����Ի���Ĳ��裺
	 * 	1.����alertDialog��builder.  
	 *	2.Ҫ��builder��������, �Ի��������,��ʽ,��ť
	 *	3.ͨ��builder ����һ���Ի���
	 *	4.�Ի���show()����  
	 */
	protected void showUpdataDialog(String description, final String url) {
		AlertDialog.Builder builer = new Builder(this) ; 
		builer.setTitle("�汾����");
		builer.setMessage(description);
		//����ȷ����ťʱ�ӷ����������� �µ�apk Ȼ��װ 
		builer.setPositiveButton("ȷ��", new OnClickListener() {
		public void onClick(DialogInterface dialog, int which) {
				Log.i(Common.TAG,"COFFIRM button is clicked!+"+VersionManager.GetUpdateInfo().getType());
				if( true == VersionManager.GetUpdateInfo().getType().equals("patch") )
				{
					DataTransfer.downLoadApkOrPatch(VersionManager.GetUpdateInfo().getUrl(), Common.newVersionPatch, Common.context, handler);
				}
				else
				{
					DataTransfer.downLoadApkOrPatch(VersionManager.GetUpdateInfo().getUrl(), Common.newVersionApk, Common.context, handler);
				}
			}   
		});
		//����ȡ����ťʱ���е�¼
		builer.setNegativeButton("ȡ��", new OnClickListener() {
			public void onClick(DialogInterface dialog, int which) {
				// TODO Auto-generated method stub
				Log.i(Common.TAG,"CANCEL button is clicked!");
				LoginMain();
			}
		});
		AlertDialog dialog = builer.create();
		dialog.show();
	}
	
	/*
	 * ��������������
	 */
	private void LoginMain(){
		Intent intent = new Intent(this,MainActivity.class);
		startActivity(intent);
		//��������ǰ��activity 
		this.finish();
	}
	
	
	
	//��װapk 
	protected void installApk(File file) {
		Intent intent = new Intent();
		//ִ�ж���
		intent.setAction(Intent.ACTION_VIEW);
		//ִ�е���������
		intent.setDataAndType(Uri.fromFile(file), "application/vnd.android.package-archive");
		startActivity(intent);
	}
	
	

}
