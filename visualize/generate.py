import webbrowser
import json
import random
with open("data.txt") as f:
    data = [i.split(",") for i in f.read().splitlines()[1:] if i.strip()!=""]


data = [{
         'algorithm':i[0].strip(),
         'vectorSize':int(i[1]),
         'numVectors':int(i[2]),
         'numIterations':int(i[3]),
         'float':float(i[4]),
         'double':float(i[5]),
         } for i in data]
    
print data

def filter(key,algorithm,numVectors):
    return [(d['vectorSize'],d[key]) for d in data if (d['algorithm']==algorithm and d['numVectors']==numVectors)]

def randomColorGenerator():
    return '#' + "".join(["%x" % random.randint(0,255) for i in range(3)])


datasets = []

for floatingPointType in ('float','double'):
    for algorithm in ('single buffer','buffer of buffers'):
        for numVectors in (100000,):
            datasets.append({
                'label': "{0}, {1}, {2} vectors".format(floatingPointType,algorithm,numVectors),
                'data': [{'x':i[0],'y':i[1]} for i in filter(floatingPointType,algorithm,numVectors)],
            }) 

rows = [[row[0]['x']]+[i['y'] for i in row] for row in zip(*[d['data'] for d in datasets])]

columDefinitions = "\n".join(["data.addColumn('number', '"+i['label']+"');" for i in datasets])



reportContents = """
<html><head></head><body>

  <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
  <div id="chart_div" style="height: 800px"></div>
      
    
<script>

google.charts.load('current', {packages: ['corechart', 'line']});
google.charts.setOnLoadCallback(drawBasic);


function drawBasic() {

      var data = new google.visualization.DataTable();
      data.addColumn('number', 'X');
      """+columDefinitions+"""
      data.addRows("""+json.dumps(rows)+""");

      var options = {
        hAxis: {
          title: 'Vector Size'
        },
        vAxis: {
          title: 'Time'
        }
      };

      var chart = new google.visualization.LineChart(document.getElementById('chart_div'));

      chart.draw(data, options);
    }
</script>


</body>
</html>

"""

with open("report.html","w") as f:
    f.write(reportContents);

webbrowser.open("report.html")