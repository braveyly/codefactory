package cn.com.service;

import javax.jws.WebParam;
import javax.jws.WebService;

@WebService
public interface IHelloWorld {
        //@WebParam(name="arg0")���п��ޣ�Ϊ����ǿ�ɶ���
	public String sayHello(@WebParam(name="arg0")String text);
}