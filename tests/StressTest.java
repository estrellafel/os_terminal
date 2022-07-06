package tests;
import java.io.BufferedWriter;
import java.io.FileWriter;

class StressTest {
    public static void main(String[] args) {
        String[] words = {"ls", "date", "whoami", "ps", "w"};
        try {
            FileWriter fw = new FileWriter("./my_test05.txt");
            BufferedWriter bw = new BufferedWriter(fw);
            for (int i = 0; i < 10000; i++) {
                bw.write(words[(int) (Math.random() * words.length)]);
                bw.newLine();
            }
            bw.close();
        } catch (Exception e) {
            System.out.print(e);
        }
    }
}
