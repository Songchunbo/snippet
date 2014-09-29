package org.myapp;

import org.myapp.spring.Action;
import org.springframework.context.ApplicationContext;
import org.springframework.context.support.FileSystemXmlApplicationContext;

public class HelloSpringMain {

	/**
	 * @param args
	 */
	public static void main(String[] args) 
	{
		// TODO Auto-generated method stub
        ApplicationContext ctx = new FileSystemXmlApplicationContext("bean.xml");
        
        Action action = (Action)ctx.getBean("Upper");
        
        System.out.println(action.execute("   Songchunbo"));
        
        Action action2 = (Action)ctx.getBean("Lower");
        System.out.println(action2.execute(" songchunbo"));
	}

}
