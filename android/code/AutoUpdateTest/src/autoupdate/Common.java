package autoupdate;

import android.content.Context;

public class Common {
	public final static int DOWNLOAD_APK_ERROR = 1;
	public final static int DOWNLOAD_APK_OK = 2;
	public final static int DOWNLOAD_XML_OK = 3;
	public final static int DOWNLOAD_XML_ERROR = 4;
	public final static String TAG = "liwei";
	public static String bspatchPath = "/mnt/sdcard/bspatch";
	public static String oldVersionApk = "/data/app/com.example.helloworld-1.apk";
	public static String newVersionApk = "/mnt/sdcard/helloworld_new.apk";
	public static String newVersionPatch = "/mnt/sdcard/hwlloworld_patch.patch";
	public static String xmlPath = "/mnt/sdcard/update.xml";
	public static String bspatchCmd = bspatchPath + " " + oldVersionApk + " " + newVersionApk + " " + newVersionPatch;
	public static String localVersion = null;
	public static String serverUrl = null;
	public static Context context = null;
	
}