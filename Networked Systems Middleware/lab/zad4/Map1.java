import java.io.IOException;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;

public class Map1 extends MapReduceBase implements Mapper<LongWritable, Text, Text, Text> {

    public void map(LongWritable key, Text value, OutputCollector<Text, Text> output, Reporter reporter) throws IOException {

        String[] parts = (value.toString()).split(" ");
        Text outputK = new Text();
        Text outputV = new Text();

        if (parts[0].equals("a")) {
            outputK.set(parts[2]);
            outputV.set("a " + parts[1] + " " + parts[3]);
            output.collect(outputK, outputV);
        } else {
            outputK.set(parts[1]);
            outputV.set("b " + parts[2] + " " + parts[3]);
            output.collect(outputK, outputV);
        }
    }
}
