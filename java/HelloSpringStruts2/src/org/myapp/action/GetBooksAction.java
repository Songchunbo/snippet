package org.myapp.action;

import org.myapp.data.BookService;

import com.opensymphony.xwork2.ActionContext;

public class GetBooksAction 
{
    private String[] books;
    
    public void setBooks(String[] books)
    {
    	this.books = books;
    }
    
    public String[] getBooks()
    {
    	return books;
    }
    
    public String execute() throws Exception
    {
    	String user = (String)ActionContext.getContext().getSession().get("user");
    	
    	if (user != null && user.equals("scott"))
    	{
    		BookService bs = new BookService();
    		setBooks(bs.getBooks());
    		return "success";
    	}
    	else
    	{
    		return "login";
    	}
    }
}
