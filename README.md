## clion  
## wsl  
## docker  
## drogon  
## odb  
## odbc  
## postman  
## vscode  
## navicat  
## hex reader 
## build2

``` javascript
[MSSQL]
Description = MS SQL Server  
Driver = ODBC Driver 18 for SQL Server  
Server = 192.168.41.234  
Database = test  
Port = 1433  
User = sa  
Password = mypass  
TrustServerCertificate = Yes  

sqlcmd -S 192.168.41.234 -U sa -C  
SELECT name FROM sys.databases;  
GO  
isql -v MSSQL sa mypass  
powershell  
New-NetFirewallRule -DisplayName "Allow SQL Server" -Direction Inbound -Action Allow -Protocol TCP -LocalPort 1433  
```  

# Build2
``` javascript
$ cd ~/build2-build

$ curl -sSfO https://download.build2.org/0.17.0/build2-install-0.17.0.sh

$ shasum -a 256 -b build2-install-0.17.0.sh

  b84e4114c61aa94c3f6278f010a0dc0536dda65ac39d3863152ec9b64510b86e

$ sh build2-install-0.17.0.sh
```

https://www.codesynthesis.com/products/odb/doc/install-build2.xhtml
/usr/local/bin/odb

c++ -o main main.o person-odb.o -L/usr/local/bin/odb -lodb-mssql -lodb

export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

permanent
echo 'export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
