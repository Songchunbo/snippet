package org.myapp.service;

public class MyServiceImpl implements MyService
{
    public boolean valid(String username, String pass)
    {
    	if (username.equals("scott") && pass.equals("tiger"))
    	{
    		return true;
    	}
    	
    	return false;
    }
}
