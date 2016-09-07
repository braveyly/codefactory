package autoupdate;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;

import android.util.Log;

public class ExecShellCmd {
	/**
	 * ����������ִ��shell����
	 * ����:cmdΪ������������ַ���
	 * ����ֵ����
	 */	
	public static void execCmd(String cmd) throws IOException {
		Log.i(Common.TAG, "Exe Commond:"+cmd);
		Runtime runtime =Runtime.getRuntime();
		Process process =runtime.exec(cmd);
		InputStream is = process.getInputStream();
		InputStreamReader  isr = new InputStreamReader(is);
		BufferedReader br = new BufferedReader(isr);
		String line = null;
		
		while(null!=(line=br.readLine())){
			Log.e(Common.TAG, line);
		}
		
		try{
			process.waitFor();
		}catch(InterruptedException e){
			e.printStackTrace();
		}	
	}
	
	/**
	 * �����������洢patch���ߵ�����
	 * ����:cmdΪ������������ַ���
	 * ����ֵ����
	 * @return 
	 */	
	public static void persistPatchTool(InputStream input)
	{
		String exe_path = Common.bspatchPath;
		File exe_file = null;
		InputStream myInput = null;
		OutputStream myOutput = null;
		byte[] buffer = new byte[1024];
		int length = 0;
		
		try {
			    myOutput = new FileOutputStream(exe_path);
			    myInput = input;
			    length = myInput.read(buffer);
			    while(length > 0)
			    {
				    myOutput.write(buffer, 0, length);
				    length = myInput.read(buffer);
			    }
		        
			    myOutput.flush();
			    myInput.close();
			    myOutput.close();
			    Log.i(Common.TAG,"Succeed to save bspatch into /mnt/sdcard");
		} catch (Exception e) {
			// TODO Auto-generated catch block
			Log.i(Common.TAG,"Failed to save bspatch into /mnt/sdcard");
			e.printStackTrace();
		} 
	}
	
	/**
	 * ��������������patch���ߵĿ�ִ��Ȩ��
	 * ����:exePathΪ���ߵ�·��
	 * ����ֵ����
	 */	
	public static void setPermission(String exePath)
	{
		
		File exeFile = null;
		exeFile = new File(exePath);
		exeFile.setExecutable(true, true);
        //Todo:�Ƿ�ر��ļ����
		Log.i(Common.TAG,"Set permission for exe file:"+exePath);
	}
	
}
