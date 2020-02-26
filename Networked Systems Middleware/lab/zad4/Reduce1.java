import java.io.IOException;
import java.util.*;
import java.util.AbstractMap.SimpleEntry;
import java.util.Map.Entry;
import java.util.Iterator;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;

public class Reduce1 extends MapReduceBase implements Reducer<Text, Text, Text, Text> {

    public void reduce(Text key, Iterator<Text> values, OutputCollector<Text, Text> output, Reporter reporter) throws IOException {

        String[] value;
        String i, k;
        ArrayList<Entry<Integer, Integer>> listA = new ArrayList<Entry<Integer, Integer>>();
        ArrayList<Entry<Integer, Integer>> listB = new ArrayList<Entry<Integer, Integer>>();

        while(values.hasNext()) {
            value = (values.next()).toString().split(" ");
            if (value[0].equals("a")) {
                listA.add(new SimpleEntry<Integer, Integer>(Integer.parseInt(value[1]), Integer.parseInt(value[2])));
            }
            else {
                listB.add(new SimpleEntry<Integer, Integer>(Integer.parseInt(value[1]), Integer.parseInt(value[2])));
            }
        }
        
        int a_ij, b_jk;
        Text outputV = new Text();
        for (Entry<Integer, Integer> a : listA) {
            i = Integer.toString(a.getKey());
            a_ij = a.getValue();
            for (Entry<Integer, Integer> b : listB) {
                k = Integer.toString(b.getKey());
                b_jk = b.getValue();
                outputV.set(i + " " + k + " " + Integer.toString(a_ij*b_jk));
                output.collect(null, outputV);
            }
        }
    }
}
