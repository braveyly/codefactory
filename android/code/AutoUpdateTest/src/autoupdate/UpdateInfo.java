package autoupdate;

import android.util.Log;

public class UpdateInfo {
	private String version;
	private String url;
	private String description;
	private String type;
	public String getVersion() {
		return version;
	}
	public void setVersion(String version) {
		this.version = version;
	}
	public String getUrl() {
		return url;
	}
	public void setUrl(String url) {
		this.url = url;
	}
	public String getDescription() {
		return description;
	}
	
	public void setDescription(String description) {
		this.description = description;
	}
	
	public String getType() {
		return type;
	}
	
	public void setType(String type) {
		this.type = type;
	}
	public void dump()
	{
		Log.i(Common.TAG, "Dump:version is "+version+" url is "+url+" description is "+ description+" type is "+ type);
	}
}

