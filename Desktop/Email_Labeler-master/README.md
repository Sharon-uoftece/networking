# EmailLabler frontend and backend

## EmailLabeler frontend
The emailLabeler frontend currently consists of four pages. 
main page
survey page
label page
reward page

### Main page features
![main page](https://github.com/Siyuan-uoftece/Email_Labeler/blob/master/Screen%20Shot%202022-12-19%20at%209.54.22%20PM.png)
* quick start button (to be replaced by LogIn/SignIn button)
* on-page video tutorial 
* model performance line chart
* labelling history fetcher

### Survey page feature
![survey page](https://github.com/Siyuan-uoftece/Email_Labeler/blob/master/Screen%20Shot%202022-12-19%20at%209.54.36%20PM.png)
This page allows user to choose the number of email to label for this time. The min is set to 1 and the max is set to 10.

### Label page feature
![label page](https://github.com/Siyuan-uoftece/Email_Labeler/blob/master/Screen%20Shot%202022-12-19%20at%209.54.52%20PM.png)
This page allows user to identify each email as sensitvie or not-sensitive using given information. Given information includes DateSent, SenderName, RecipientName, EmailSubject, RecipientCount, FileSensitiveCount, FileSize and DaySinceHire. To label each email, simply click the sensitive button if you suspecct the email to contain sensitive information, click the non-sensitive button otherwise. There is also a confidence slider bar for you to mark your confidence level of such choice. 

### Reward page feature
![reward page](https://github.com/Siyuan-uoftece/Email_Labeler/blob/master/Screen%20Shot%202022-12-19%20at%209.54.09%20PM.png)
This page shows how many email you have correctly label, for each correctly labeled email, the user will get one reward point.



