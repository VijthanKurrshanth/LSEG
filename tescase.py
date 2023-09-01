import random
import csv

def generate_test_case(i):
  """Generates a random test case in CSV format."""
  client_order_id = "aa" + str(i)
  instrument = random.choice(["Rose", "Lavender", "Tulip", "Orchid", "Lotus"])
  side = random.choice(["1", "2"])
  quantity = random.randint(100, 300)
  while quantity % 10 != 0:
    quantity = random.randint(100, 300)
  price = random.randint(1, 1000)
  return [client_order_id, instrument, side, quantity, price]

def main():
  """Generates and saves a large test case in CSV format."""
  test_case = []
  i = 1
  for _ in range(200000):
    test_case.append(generate_test_case(i))
    i += 1
  with open("orders.csv", "w", newline="") as csvfile:
    writer = csv.writer(csvfile, delimiter=",")
    writer.writerow(["orders.csv"])
    writer.writerow(["Client Order ID", "Instrument", "Side", "Quantity", "Price"])
    for row in test_case:
      writer.writerow(row)

if __name__ == "__main__":
  main()
