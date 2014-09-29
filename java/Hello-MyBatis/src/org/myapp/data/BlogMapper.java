package org.myapp.data;


public interface BlogMapper 
{
    public Blog selectBlog(int id);
}
/*
<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE mapper
  PUBLIC "-//mybatis.org//DTD Mapper 3.0//EN"
  "http://mybatis.org/dtd/mybatis-3-mapper.dtd">
<mapper namespace="org.mybatis.example.BlogMapper">
  <select id="selectBlog" parameterType="int" resultType="Blog">
    select * from Blog where id = #{id}
  </select>
</mapper>
*/
/*
package com.songchunbo.HelloMybatis;

public class HelloMybatisMain 
{
    public static void main(String [] args)
    {
    	System.out.println("Hello MybatisMain");
    	
    	String resource = "mybaits-config.xml";
    	
    	InputStream inputStream = Resources.getResourceAsStream(resource);
    	
    	sqlSessionFactory = new SqlSessionFactoryBuilder().build(inputStream);
    	
    	Session session = sqlSessionFactory.openSession();
    	
    	try 
    	{
    		Blog blog = session.selectOne("selectBlog", 101);
    		
    	}
    	finally
    	{
    		session.close();
    	}
    	
    	
    	SqlSession session = sqlSessionFactory.openSession();
    	try {
    	  BlogMapper mapper = session.getMapper(BlogMapper.class);
    	  Blog blog = mapper.selectBlog(101);
    	} finally {
    	  session.close();
    	}
    }
}
*/