package pro.webws.client;

import org.springframework.context.ApplicationContext;
import org.springframework.context.support.ClassPathXmlApplicationContext;
import cn.com.service.IHelloWorld;

public class Test {
	public static void main(String[] args) {
		ApplicationContext ctx = new ClassPathXmlApplicationContext("spring-client.xml");
		IHelloWorld client = (IHelloWorld) ctx.getBean("client");
		String result = client.sayHello("ÄãºÃ!");
		System.out.println(result);
	}
}