from openpyxl import load_workbook
import csv
workbook = load_workbook('20min_data.xlsx')
#booksheet = workbook.active
sheets = workbook.get_sheet_names()
booksheet = workbook.get_sheet_by_name(sheets[0])
 
rows = booksheet.rows
columns = booksheet.columns
temp=[]
for row in rows:
    line = [col.value for col in row]
    print (row[0].value)
    v=str(row[0].value).split(':')
    print(str(int(v[0])*60+int(v[1])));
    temp.append([str(int(v[0])*60+int(v[1]))])

with open("time.csv","w") as csvfile: 
    writer = csv.writer(csvfile)
    writer.writerows(temp)
'''
with open("mapOfAddress.csv","r") as csvfile:
    reader = csv.reader(csvfile)
    index=0
    rows=[]
    for line in reader:
	#print line
	if line[2]=='Match':
            temp=[line[0],line[1],line[2],line[3],line[4],line[8].zfill(2)+line[9].zfill(3)+line[10].zfill(6)+line[11][0]]
	    print temp
	else:
	    temp=[line[0],line[1],line[2],line[3]]
	    print temp
	index=index+1
	rows.append(temp)
	#print(temp)
    #print rows
    with open("out.csv","w") as csvfile: 
    	writer = csv.writer(csvfile)
    	writer.writerows(rows)
with open("data_simple.csv","r") as csvfile:
    reader = csv.reader(csvfile)
    index=0
    rows=[]
    for line in reader:
	#print line
	if line[14]=='Match':
	    if line[0]=='Active':
		temp=['1',line[13],line[5],line[13].zfill(12)]
	    else:
		temp=['0',line[13],line[5],line[13].zfill(12)]
	    print temp
	    rows.append(temp)
	#print(temp)
    #print rows
    with open("label.csv","w") as csvfile: 
    	writer = csv.writer(csvfile)
    	writer.writerows(rows)
'''
