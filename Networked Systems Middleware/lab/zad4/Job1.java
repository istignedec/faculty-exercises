import java.io.IOException;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.FileInputFormat;
import org.apache.hadoop.mapred.FileOutputFormat;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;

public class Job1 {

    public static void main(String[] args) throws IOException {

        if (args.length != 2) {
            System.err.println("Usage: Job1 <input path> <output path>");
            System.exit(-1);
        }

        JobConf conf = new JobConf(Job1.class);
        conf.setJobName("Job1");
        FileInputFormat.addInputPath(conf, new Path(args[0]));
        FileOutputFormat.setOutputPath(conf, new Path(args[1]));
        conf.setMapperClass(Map1.class);
        conf.setReducerClass(Reduce1.class);
        conf.setOutputKeyClass(Text.class);
        conf.setOutputValueClass(Text.class);
        JobClient.runJob(conf);
    }
}
