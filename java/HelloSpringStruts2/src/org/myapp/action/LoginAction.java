package org.myapp.action;

import org.myapp.service.MyService;

import com.opensymphony.xwork2.ActionContext;

public class LoginAction 
{
    private String username;
    private String password;
    
    private String tip;
    
    private MyService ms;
    
    public void setMs(MyService ms)
    {
    	this.ms = ms;
    }
    
    public void setUsername(String username)
    {
    	this.username = username;
    }
    
    public String getUsername()
    {
    	return this.username;
    }
    
    public void setPassword(String password)
    {
    	this.password = password;
    }
    
    public String getPassword()
    {
    	return this.password;
    }
    
    public void setTip(String tip)
    {
    	this.tip = tip;
    }
    
    public String getTip()
    {
    	return this.tip;
    }
    
    
    public String execute() throws Exception
    {
    	if (ms.valid(getUsername(), getPassword()))
    	{
    		setTip("哈哈,整合成功! ");
    		ActionContext.getContext().getSession().put("user", getUsername());
    		return "success";
    	}
    	else
    	{
    		return "error";
    	}
    }
}
