package com.way.weather;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;

import javax.net.ssl.HttpsURLConnection;

import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;
import org.jdom2.Document;
import org.jdom2.Element;
import org.jdom2.JDOMException;
import org.jdom2.input.SAXBuilder;

import android.app.Dialog;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.view.ViewPager;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.baidu.location.BDLocation;
import com.baidu.location.BDLocationListener;
import com.baidu.location.LocationClient;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.reflect.TypeToken;
import com.way.apapter.WeatherPagerAdapter;
import com.way.app.Application;
import com.way.bean.City;
import com.way.bean.WeatherData;

import com.way.db.CityDB;
import com.way.fragment.FirstWeatherFragment;
import com.way.fragment.SecondWeatherFragment;
import com.way.indicator.CirclePageIndicator;
import com.way.util.ConfigCache;
import com.way.util.IphoneDialog;
import com.way.util.L;
import com.way.util.NetUtil;
import com.way.util.SharePreferenceUtil;
import com.way.util.T;
import com.way.util.TimeUtil;

public class MainActivity extends FragmentActivity implements
		Application.EventHandler, OnClickListener {
	public static final String UPDATE_WIDGET_WEATHER_ACTION = "com.way.action.update_weather";
	private static final int LOACTION_OK = 0;
	private static final int ON_NEW_INTENT = 1;
	private static final int UPDATE_EXISTS_CITY = 2;
	private static final int GET_WEATHER_RESULT = 3;
	private LocationClient mLocationClient;
	private CityDB mCityDB;
	private SharePreferenceUtil mSpUtil;
	private Application mApplication;
	private City mCurCity;
	private Gson mGson;
	private ImageView mCityManagerBtn, mUpdateBtn, mLocationBtn, mShareBtn;
	private ProgressBar mUpdateProgressBar;
	private TextView mTitleTextView;
	private City mNewIntentCity;
	private WeatherPagerAdapter mWeatherPagerAdapter;
	private static WeatherData weatherdata;
	
	private TextView cityTv, timeTv, humidityTv, weekTv, pmDataTv, pmQualityTv,
			temperatureTv, climateTv, windTv;
	private ImageView weatherImg, pmImg;;
	private ViewPager mViewPager;
	private List<Fragment> fragments;

	private Handler mHandler = new Handler() {
		public void handleMessage(android.os.Message msg) {
			switch (msg.what) {
			case LOACTION_OK:

				String cityName = (String) msg.obj;
				L.i("cityName = " + cityName);
				mCurCity = mCityDB.getCity(cityName);
				L.i(mCurCity.toString());
				mSpUtil.setCity(mCurCity.getCity());
				cityTv.setText(mCurCity.getCity());
				//updateWeather(true);
				break;
			case ON_NEW_INTENT:
				mCurCity = mNewIntentCity;
				mSpUtil.setCity(mCurCity.getCity());
				cityTv.setText(mCurCity.getCity());
				getWeatherData(mCurCity.getCity());
				//updateWeather(true);
				break;
			case UPDATE_EXISTS_CITY:
				String sPCityName = mSpUtil.getCity();
				mCurCity = mCityDB.getCity(sPCityName);
				//updateWeather(false);
				break;
			case GET_WEATHER_RESULT:
				//updateWeatherInfo();
				//updatePm2d5Info();
				//updateWidgetWeather();
				//mUpdateBtn.setVisibility(View.VISIBLE);
				//mUpdateProgressBar.setVisibility(View.GONE);
				updateWeatherView(weatherdata);
				break;
			default:
				break;
			}
		}

	};
	
	public static String request(String httpUrl) {
		BufferedReader reader = null;String result = null;StringBuffer sbf = new StringBuffer();
		try {
				URL url = new URL(httpUrl);
				HttpURLConnection connection = (HttpURLConnection) url.openConnection();
				connection.setRequestMethod("GET");
				connection.connect();
				InputStream is = connection.getInputStream();
				reader = new BufferedReader(new InputStreamReader(is, "UTF-8"));
				String strRead = null;
				while ((strRead = reader.readLine()) != null) {
					sbf.append(strRead); sbf.append("\r\n");
				}
				reader.close();
				result = sbf.toString();
		} 
		catch (Exception e) 
		{ 
			e.printStackTrace(); 
		}
		return result;
	}
	
	public static void parseWeatherXmlResult(String xmlBuffer)
	{
		 Log.i("liwei","start parse weather xml result");
		SAXBuilder sb = new SAXBuilder();
        Document doc = null;
		try {
			InputStream is = new ByteArrayInputStream(xmlBuffer.getBytes());
			doc = sb.build(is);
		} catch (JDOMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        Element root = doc.getRootElement();
        List<Element> elements = root.getChildren();
        Log.i("liwei","root name is "+root.getName());
        for (Element element : elements) {
        	Log.i("liwei","element name is "+element.getName());
        	if( element.getName() == "forecast" )
        	{
        		List<Element> children = element.getChildren();
        		for (Element child : children) {
        			List<Element> attributeChildren = child.getChildren();
        			
        			for (Element attributeChild : attributeChildren)
        			{
	        			if( attributeChild.getName() == "date" )
	        			{
	        				Log.i("liwei","date is " + attributeChild.getText());
	        				
	        			}
	        			
	        			if( attributeChild.getName() == "high" )
	        			{
	        				Log.i("liwei","high is " + attributeChild.getText());
	        				weatherdata.Hightemp = attributeChild.getText();
	        			}
	        			
	        			if( attributeChild.getName() == "low" )
	        			{
	        				Log.i("liwei","low is " + attributeChild.getText());
	        				weatherdata.Lowtemp = attributeChild.getText();
	        			}  
	        			
	        			Log.i("liwei", "120----attributeChild.getName() is " + attributeChild.getName() );
	        			if( attributeChild.getName() == "day" )
	        			{
	        				Log.i("liwei", "110----attributeChild.getName() is " + attributeChild.getName() );
	        				List<Element> dayChild = attributeChild.getChildren();
	        				for (Element dayAttributeChild : dayChild )
	        				{
	        					Log.i("liwei","dayAttributeChild.getName() =  " + dayAttributeChild.getName());
	    	        			if( dayAttributeChild.getName() == "type" )
	    	        			{
	    	        				Log.i("liwei","day type is " + dayAttributeChild.getText());
	    	        				weatherdata.Daytype = dayAttributeChild.getText();
	    	        			}	        					
	        				}
	        			}
	        				        			
	        			if( attributeChild.getName() == "night" )
	        			{
	        				List<Element> nightChild = attributeChild.getChildren();
	        				for (Element nightAttributeChild : nightChild )
	        				{
	        					Log.i("liwei","nightAttributeChild.getName() =  " + nightAttributeChild.getName());
	    	        			if( nightAttributeChild.getName() == "type" )
	    	        			{
	    	        				Log.i("liwei","night type is " + nightAttributeChild.getText());
	    	        				weatherdata.Nighttype = nightAttributeChild.getText();
	    	        			}	        					
	        				}
	        			}
        			}
        			break;
        		}
        	}
        	
        	
        	if( element.getName() == "environment" )
        	{
        		List<Element> children = element.getChildren();
        		for (Element child : children) {
        			if( child.getName() == "pm25" )
        			{
        				Log.i("liwei", "pm25 is " + child.getText());
        				weatherdata.Pm25 = child.getText();
        			}
        			
        			if( child.getName() == "quality" )
        			{
        				Log.i("liwei", "quality is " + child.getText());
        				weatherdata.Quality = child.getText();
        			}
        		}
        	}
        	
        	if( element.getName() == "city" )
        	{
        		Log.i("liwei", "city is " + element.getText());
        		weatherdata.City =  element.getText();
        	}
        	
        	if( element.getName() == "wendu" )
        	{
        		Log.i("liwei", "wendu is " + element.getText());
        		weatherdata.Wendu =  element.getText();
        	}
        	
        	if( element.getName() == "fengli" )
        	{
        		Log.i("liwei", "fengli is " + element.getText());
        		weatherdata.Fengli =  element.getText();
        	}
        	
        	if( element.getName() == "shidu" )
        	{
        		Log.i("liwei", "shidu is " + element.getText());
        		weatherdata.Shidu =  element.getText();
        	}
        	
        	if( element.getName() == "fengxiang" )
        	{
        		Log.i("liwei", "fengxiang is " + element.getText());
        		weatherdata.Fengxiang =  element.getText();
        	}
        	
        	if( element.getName() == "sunrise_1" )
        	{
        		Log.i("liwei", "sunrise_1 is " + element.getText());
        		weatherdata.Sunrise =  element.getText();
        	}
        	
        	if( element.getName() == "sunset_1" )
        	{
        		Log.i("liwei", "sunset_1 is " + element.getText());
        		weatherdata.Sunset =  element.getText();
        	}
        }
        
	}
	
	/**
	* Get XML String of utf-8
	* 
	* @return XML-Formed string
	*/
	public static String getUTF8XMLString(String xml) {
		// A StringBuffer Object
		StringBuffer sb = new StringBuffer();
		sb.append(xml);
		String xmString = "";
		String xmlUTF8="";
		try {
				xmString = new String(sb.toString().getBytes("UTF-8"));
				xmlUTF8 = URLEncoder.encode(xmString, "UTF-8");
				System.out.println("utf-8 编码：" + xmlUTF8) ;
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		// return to String Formed
		return xmlUTF8;
	}
	
	void getWeatherData(final String city)
	{
		new Thread()
		{
			public void run()
			{
				//http://wthrcdn.etouch.cn/WeatherApi?city=%E6%AD%A6%E6%B1%89
				String httpUrl = "http://wthrcdn.etouch.cn/WeatherApi?city=";
				String httpurl_utf8 = getUTF8XMLString(city);
				httpUrl += httpurl_utf8;
				Log.i("liwei","httpurl is " + httpUrl);
				Log.i("liwei","httpurl_utf8 is " + httpurl_utf8);
				
				String jsonResult = request(httpUrl);
				if( null == jsonResult )
				{
					Log.i("liwei", "jsonResult");
				}
				else
				{
					Log.i("liwei","jsonresult is not null"+jsonResult);
				}
				parseWeatherXmlResult(jsonResult);
				mHandler.sendEmptyMessage(GET_WEATHER_RESULT);
			}
		}.start();
		
		
	}
	
	void updateWeatherView(WeatherData weatherdata)
	{
		temperatureTv.setText(weatherdata.Hightemp+"/"+weatherdata.Lowtemp);
		cityTv.setText(weatherdata.City);
		pmDataTv.setText(weatherdata.Pm25);
		pmQualityTv.setText(weatherdata.Quality);
		windTv.setText(weatherdata.Fengxiang+"/"+weatherdata.Fengli);
		climateTv.setText(weatherdata.Daytype+"/"+weatherdata.Nighttype);
		humidityTv.setText(weatherdata.Shidu);
	}
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		weatherdata = new WeatherData();
		initData();
		initView();
		getWeatherData("wuhan");

	}

	private void startActivityForResult() {
		Intent i = new Intent(this, SelectCtiyActivity.class);
		startActivityForResult(i, 0);
	}

	private void initView() {
		mCityManagerBtn = (ImageView) findViewById(R.id.title_city_manager);
		mUpdateBtn = (ImageView) findViewById(R.id.title_update_btn);
		mShareBtn = (ImageView) findViewById(R.id.title_share);
		mLocationBtn = (ImageView) findViewById(R.id.title_location);
		mCityManagerBtn.setOnClickListener(this);
		mUpdateBtn.setOnClickListener(this);
		mShareBtn.setOnClickListener(this);
		mLocationBtn.setOnClickListener(this);
		mShareBtn.setVisibility(View.GONE);
		mUpdateProgressBar = (ProgressBar) findViewById(R.id.title_update_progress);
		mTitleTextView = (TextView) findViewById(R.id.title_city_name);

		cityTv = (TextView) findViewById(R.id.city);
		timeTv = (TextView) findViewById(R.id.time);
		timeTv.setText(TimeUtil.getDay(mSpUtil.getTimeSamp())
				+ mSpUtil.getTime() + "发布");
		humidityTv = (TextView) findViewById(R.id.humidity);
		weekTv = (TextView) findViewById(R.id.week_today);
		weekTv.setText("今天 " + TimeUtil.getWeek(0, TimeUtil.XING_QI));
		pmDataTv = (TextView) findViewById(R.id.pm_data);
		pmQualityTv = (TextView) findViewById(R.id.pm2_5_quality);
		pmImg = (ImageView) findViewById(R.id.pm2_5_img);
		temperatureTv = (TextView) findViewById(R.id.temperature);
		temperatureTv.setText("13摄氏度");
		climateTv = (TextView) findViewById(R.id.climate);
		windTv = (TextView) findViewById(R.id.wind);
		weatherImg = (ImageView) findViewById(R.id.weather_img);
		fragments = new ArrayList<Fragment>();
		fragments.add(new FirstWeatherFragment());
		fragments.add(new SecondWeatherFragment());
		mViewPager = (ViewPager) findViewById(R.id.viewpager);
		mWeatherPagerAdapter = new WeatherPagerAdapter(
				getSupportFragmentManager(), fragments);
		mViewPager.setAdapter(mWeatherPagerAdapter);
		((CirclePageIndicator) findViewById(R.id.indicator))
				.setViewPager(mViewPager);
		if (TextUtils.isEmpty(mSpUtil.getCity())) {
			if (NetUtil.getNetworkState(this) != NetUtil.NETWORN_NONE) {
				mLocationClient.start();
				mLocationClient.requestLocation();
				T.showShort(this, "正在定位...");
				mUpdateBtn.setVisibility(View.GONE);
				mUpdateProgressBar.setVisibility(View.VISIBLE);
			} else {
				T.showShort(this, R.string.net_err);
			}
		} else {
			//mHandler.sendEmptyMessage(UPDATE_EXISTS_CITY);
		}
	}

	private void initData() {
		Application.mListeners.add(this);
		mApplication = Application.getInstance();
		mSpUtil = mApplication.getSharePreferenceUtil();
		mLocationClient = mApplication.getLocationClient();

		mLocationClient.registerLocationListener(mLocationListener);
		mCityDB = mApplication.getCityDB();
		// 不转换没有 @Expose 注解的字段
		mGson = new GsonBuilder().excludeFieldsWithoutExposeAnnotation()
				.create();
	}

	BDLocationListener mLocationListener = new BDLocationListener() {

		@Override
		public void onReceivePoi(BDLocation arg0) {
			// do nothing
		}

		@Override
		public void onReceiveLocation(BDLocation location) {
			// mActionBar.setProgressBarVisibility(View.GONE);
			mUpdateBtn.setVisibility(View.VISIBLE);
			mUpdateProgressBar.setVisibility(View.GONE);
			if (location == null || TextUtils.isEmpty(location.getCity())) {
				// T.showShort(getApplicationContext(), "location = null");
				final Dialog dialog = IphoneDialog.getTwoBtnDialog(
						MainActivity.this, "定位失败", "是否手动选择城市?");
				((Button) dialog.findViewById(R.id.ok))
						.setOnClickListener(new OnClickListener() {

							@Override
							public void onClick(View v) {
								startActivityForResult();
								dialog.dismiss();
							}
						});
				dialog.show();
				return;
			}
			String cityName = location.getCity();
			mLocationClient.stop();
			Message msg = mHandler.obtainMessage();
			msg.what = LOACTION_OK;
			msg.obj = cityName;
			mHandler.sendMessage(msg);// 更新天气
		}
	};

	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == 0 && resultCode == RESULT_OK) {
			mNewIntentCity = (City) data.getSerializableExtra("city");
			mHandler.sendEmptyMessage(ON_NEW_INTENT);
		}
	}

	@Override
	public void onCityComplite() {
		// do nothing
	}

	@Override
	public void onNetChange() {
		if (NetUtil.getNetworkState(this) == NetUtil.NETWORN_NONE)
			T.showLong(this, R.string.net_err);
		// else if (!TextUtils.isEmpty(mSpUtil.getCity())) {
		// String sPCityName = mSpUtil.getCity();
		// mCurCity = mCityDB.getCity(sPCityName);
		// getWeatherInfo(true, true);
		// }
	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch (v.getId()) {
		case R.id.title_city_manager:
			startActivityForResult();
			break;
		case R.id.title_location:
			if (NetUtil.getNetworkState(this) != NetUtil.NETWORN_NONE) {
				if (!mLocationClient.isStarted())
					mLocationClient.start();
				mLocationClient.requestLocation();
				T.showShort(this, "正在定位...");
			} else {
				T.showShort(this, R.string.net_err);
			}
			break;
		case R.id.title_share:
			if (NetUtil.getNetworkState(this) != NetUtil.NETWORN_NONE) {
				// do something
			} else {
				T.showShort(this, R.string.net_err);
			}
			break;
		case R.id.title_update_btn:
			if (NetUtil.getNetworkState(this) != NetUtil.NETWORN_NONE) {
				if (TextUtils.isEmpty(mSpUtil.getCity())) {
					T.showShort(this, "请先选择城市或定位！");
				} else {
					String sPCityName = mSpUtil.getCity();
					mCurCity = mCityDB.getCity(sPCityName);
					//updateWeather(true);
				}
			} else {
				T.showShort(this, R.string.net_err);
			}
			break;

		default:
			break;
		}
	}

}
