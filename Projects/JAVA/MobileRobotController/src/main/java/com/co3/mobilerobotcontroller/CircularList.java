package com.co3.mobilerobotcontroller;

import java.util.ArrayList;

public class CircularList<E> extends ArrayList<E> {

    public CircularList(int i) {
        super(i);
    }

    @Override
    public E get(int index) {
        return super.get(index % size());
    }
}