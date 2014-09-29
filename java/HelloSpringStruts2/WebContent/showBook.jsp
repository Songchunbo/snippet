<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<%@ page import="java.util.*, com.opensymphony.xwork2.util.*" %>    
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>all the book</title>
</head>
<body>
     <table border="1" width="360">
         <caption>all the book</caption>
         <%
             ValueStack vs = (ValueStack)request.getAttribute("struts.valueStack");
         
             String[] books = (String[])vs.findValue("books");
             
             for (String book: books)
             {
          %>
          <tr>
              <td>书名: </td>
              <td><%=book %></td>
          </tr>
          <%  } %>    
     </table>
</body>
</html>