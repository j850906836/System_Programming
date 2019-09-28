我的作法是acp file1 路徑
然後一開始會是在file1的那個資料夾執行
file1只是單純的檔案名稱，沒有包含路徑
file2就是只有路徑，如:/home/Desktop/2018-sp

狀況1:
ex: ./acp test.txt /home/Dektop/2018-sp

如果file2的路徑是存在的，就會在file2的那個路徑資料夾製造出tmpFile
之後會先暫停，讓助教觀察tmpfile，按Enter就可以繼續
之後會把tmpFile改名成file1的那個檔名，結束程式

狀況2:
ex: ./acp test.txt tmp.txt

若是file2放的並不是有效的路徑，比如說是tmp.txt
那就會在file1那個資料夾做出tmpFile，助教也可以觀察
之後會把tmpFile改名成file2那個名稱
