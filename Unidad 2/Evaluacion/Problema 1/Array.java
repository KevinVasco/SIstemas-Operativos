import java.util.Iterator;
import java.util.NoSuchElementException;

/*
*   Este codigo es de autoria de Robert Sedgewick y Kevin Wayne.
*   Copyright © 2000–2019, Robert Sedgewick and Kevin Wayne. 
*   Recuperado de:  https://algs4.cs.princeton.edu/13stacks/Queue.java.html
*/
public class Array<Item> implements Iterable<Item> {
    private Node<Item> first;
    private Node<Item> last;
    private int len;

    private static class Node<Item> {
        private Item item;
        private Node<Item> next;
    }

    @Override
    public Iterator<Item> iterator() {
        return new LinkedIterator(first);
    }

    private class LinkedIterator implements Iterator<Item> {
        private Node<Item> current;

        public LinkedIterator(Node<Item> first) {
            current = first;
        }

        @Override
        public boolean hasNext() {
            return current != null;
        }

        @Override
        public Item next() {
            if (!hasNext()) throw new NoSuchElementException();
            Item item = current.item;
            current = current.next;
            return item;
        }
    }

    public Array() {                                            //  IntArray *createIntArray(int size);
        first = null;
        last = null;
        len = 0;
    }

    public boolean isEmpty() {
        return first == null;
    }

    public int size() {
        return len;
    }

    public Item peek() {
        if (isEmpty()) throw new NoSuchElementException("Array underflow");
        return first.item;
    }

    public void addToArray(Item item) {
        Node<Item> oldlast = last;
        last = new Node<Item>();
        last.item = item;
        last.next = null;
        if (isEmpty())  first = last;
        else            oldlast.next = last;
        len++;
    }

    public Item removeFromArray() {                             //  void destroyIntArray(IntArray *);
        if (isEmpty()) throw new NoSuchElementException("Array underflow");
        Item item = first.item;
        first = first.next;
        len --;
        if (isEmpty()) last = null;
        return item;
    }

    public String toString() {                                  //  void printArray(IntArray *);
        StringBuilder s = new StringBuilder();
        for (Item item : this) {
            s.append(item);
            s.append(" ");
        }
        return s.toString();
    }

    public static void initArray(Array a, int n) {              //  void initIntArray(IntArray *);
        for (int i = 0; i < n; i++) {
            a.addToArray(i);
        }
    }

    public static void main(String[] args) {
        int n = 100;
        Array<Integer> arreglo = new Array<>();                 //  IntArray *createIntArray(int size);

        initArray(arreglo, n);                                  //  void initIntArray(IntArray *);

        System.out.println(arreglo.toString());                 //  void printArray(IntArray *);
        System.out.println("----------------");                 

        for (int i = 0; i < n; i++) {
            System.out.print(arreglo.removeFromArray() + " ");  //  void destroyIntArray(IntArray *);
        }
    }
}
