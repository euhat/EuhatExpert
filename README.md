# EuhatExpert
This is an open source code of Euhat Expert.

## Official site:　
[http://euhat.com/expert/index.php](http://euhat.com/expert/index.php) 

## It has features:
* Transfer data between Android and PC.
* Work Trace Visualization.
* Clock In & Clock Out Inform.

## Important
1. Current building in Windows only supports x86!

## How to build:
1. In euhat\dbop\DbOpSqlite.cpp, modify DB_KEY_x to your private key of sqlite db, or else others will know to decrypt your certificate keys.
1. Open Android Studio, import project using build.gradle in android\EuhatExpert dir.
2. After install vs2019, double-click Expert.sln in msvc\EuhatExpert, compile in x86 mode.

## About 3rd open sources
1. libjpeg
2. openssl

Have any question, welcome to contact me through email: euhat@hotmail.com
