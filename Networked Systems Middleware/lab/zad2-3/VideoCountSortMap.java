import java.io.IOException;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;

public class VideoCountSortMap extends MapReduceBase implements Mapper<LongWritable, Text, Text, Text> {

public void map(LongWritable key, Text value, OutputCollector<Text, Text> output, Reporter reporter) throws IOException {

String[] parts = (value.toString()).split("\t");

if(parts.length == 2) {
String noviValue = parts[0];
String noviKey = parts[1];
output.collect(new Text(noviKey), new Text(noviValue));
}
}
}
