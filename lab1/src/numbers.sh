#!/bin/bash

# Генерируем 150 случайных чисел и сохраняем в numbers.txt
for i in {1..150}; do
  # Генерируем случайное число от 0 до 100 
  echo $((RANDOM % 100)) >> numbers.txt
done