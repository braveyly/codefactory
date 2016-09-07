package cn.com.service;
import javax.jws.WebService;

@WebService(endpointInterface="cn.com.service.IHelloWorld")
public class HelloWorldImpl implements IHelloWorld {
	public String sayHello(String text) {	
		return "Hello" + text ;
	}
}