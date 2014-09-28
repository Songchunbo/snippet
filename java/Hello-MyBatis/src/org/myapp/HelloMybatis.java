package org.myapp;

import java.io.IOException;
import java.io.InputStream;

import org.apache.ibatis.io.Resources;
import org.apache.ibatis.session.SqlSession;
import org.apache.ibatis.session.SqlSessionFactory;
import org.apache.ibatis.session.SqlSessionFactoryBuilder;
import org.myapp.data.Blog;
import org.myapp.data.BlogMapper;

public class HelloMybatis 
{
    public static void main(String []args)
    {
    	BlogMapper bm;
    	String resource = "org/myapp/data/mybatis-config.xml";
    	InputStream inputStream = null;
		try {
			inputStream = Resources.getResourceAsStream(resource);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
    	SqlSessionFactory sqlSessionFactory = new SqlSessionFactoryBuilder().build(inputStream);
    	
    	
    	SqlSession session = sqlSessionFactory.openSession();
    	
    	//Blog blog = (Blog)session.selectOne(
    	//		"org.mybatis.example.BlogMapper.selectBlog", 101);
    	
    	try
    	{
    		BlogMapper mapper = session.getMapper(BlogMapper.class);
    		
    		for (int i = 1; i <= 19; ++i)
    		{
    		Blog bl = mapper.selectBlog(i);
    		
    	    System.out.println(bl.getId() + ":" + bl.getUrl());
    		}
    	}
    	finally
    	{
    		session.close();
    	}
    	System.out.println("Hello Mybatis");
    	
    	
    }
}
