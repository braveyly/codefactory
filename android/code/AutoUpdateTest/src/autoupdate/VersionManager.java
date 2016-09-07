package autoupdate;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;

import org.xmlpull.v1.XmlPullParser;

import com.example.autoupdatetest.R;
import com.example.autoupdatetest.R.bool;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.util.Log;
import android.util.Xml;

public class VersionManager {
	private static UpdateInfo updateInfo = null;
	

	public static UpdateInfo GetUpdateInfo()
	{
		return updateInfo;
	}
	
	public static UpdateInfo SetUpdateInfo(UpdateInfo info)
	{
		updateInfo = info;
		return updateInfo;
	}
	
	public static boolean CompareVersion() throws Exception
	{
		if( Common.localVersion != updateInfo.getVersion() )
		{
			return false;
		}
		
		return true;
	}
	
}
