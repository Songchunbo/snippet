package org.myapp.data;

public class BookService 
{
    private String[] books = 
    		new String[] {
    		   "JavaScript Bible" ,
    		   "构建高性能web站点",
    		   "Struts2权威指南",
    		   "Thinking in C++",
    		   "Thinking in Java",
    		   "The Art of Programming"
    };
    
    
    public String[] getBooks()
    {
    	return books;
    }
}
