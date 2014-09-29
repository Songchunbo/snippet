package org.myapp.spring;

public class UpperAction implements Action {

	private String messages;
	
	public String getMessage()
	{
		return messages;
	}
	
	public void setMessage(String string)
	{
		messages = string;
	}
	@Override
	public String execute(String str) {
		// TODO Auto-generated method stub
		return (getMessage() + str).toUpperCase();
	}

}
