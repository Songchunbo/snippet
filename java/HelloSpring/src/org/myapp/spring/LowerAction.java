package org.myapp.spring;

public class LowerAction implements Action {

	private String message;
	
	public String getMessage()
	{
		return message;
	}
	
	public void setMessage(String string)
	{
		message = string;
	}
	@Override
	public String execute(String str) {
		// TODO Auto-generated method stub
		return (getMessage()+str).toLowerCase();
	}

}
