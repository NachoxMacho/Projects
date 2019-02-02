import java.util.concurrent.ThreadLocalRandom;

/**
 * @brief Uses Threads to simulate different customers interacting with a Bank
 * @version 1.1
 * @author Rob Owens
 */
public class RunCustomer extends Thread{
	int ID;
	int[] current;
	int[] needs;
	int[] returns;
	Bank bank;
	/**
	 * @brief Constructor for the Customers which assigns a different thread for each customer
	 * @param ID - The ID of the customer(0-9)
	 * @param Resources - How many different resources the bank has
	 * @param incomingBank - What bank the customer is dealing with
	 * @author Rob Owens
	 */
	public RunCustomer(int ID, int Resources, Bank incomingBank) {
		// Creates the customer
		// Sets the ID, bank its tied to, and the vectors for its needs and what it holds
		this.ID = ID;
		this.bank = incomingBank;
		this.current = new int[Resources];
		this.needs = new int[Resources];
		this.returns = new int[Resources];
		for(int i = 0; i < Resources; i++)
			current[i] = needs[i] = returns[i] = 0;
	}
	
 	/* (non-Javadoc)
 	 * @see java.lang.Thread#run()
 	 */
 	public void run() {		
 		
 		for (int j = 0; j < 4; j++) {
 			// Sleep for 1-5 seconds
			try {sleep(ThreadLocalRandom.current().nextInt(1000, 5001));} catch (InterruptedException e) {}
			
			// Creates and outputs a needs vector that will not over reach the maximum number of resources
			System.out.print("ID#" + ID + " is requesting: [ ");
			for (int i = 0; i < this.bank.totalResources; i++) {
				// This is where the random number generator is used  in the range (0, Max customer demand - current resources +1(not inclusive upper bound))
				this.needs[i] = ThreadLocalRandom.current().nextInt(0, this.bank.customerMax[ID][i]+1 - this.current[i]);
				System.out.print(this.needs[i] + " ");
			}
			System.out.print("]\n");
			
			// Try to take the resources every half second
			try { while (!this.bank.requestResource(ID, needs)) sleep(500);} catch (InterruptedException e) {}
			
			// Once the Resources have been taken, add them to the customer's allocation vector
			for (int i = 0; i < this.bank.totalResources; i++) current[i] += needs[i];
			
			// Sleep for 1-5 seconds
			try { sleep(ThreadLocalRandom.current().nextInt(1000, 5001)); } catch (InterruptedException e) {}
			
			// Generate the number of resources that the customer will be returning
			System.out.print("ID#" + ID + " is returning: [ ");
			for (int i = 0; i < this.bank.totalResources; i++) {
				// This is where the random number generator is used  in the range (0, current resources +1(not inclusive upper bound))
				this.returns[i] = ThreadLocalRandom.current().nextInt(0, this.current[i] + 1);
				System.out.print(this.returns[i] + " ");
			}
			System.out.print("]\n");
			
			// Try to return the resources every half second
			try {while (!this.bank.releaseResource(ID, returns)) sleep(500);} catch (InterruptedException e) {}
			
			// Once the resources are returned, subtract from the current allocation vector
			for (int i = 0; i < this.bank.totalResources; i++)
				current[i] -= returns[i];
		}
 		// Return the last of the resources its using
 		System.out.print("ID#" + ID + " returning all of its resources.\n");
 		try {while (!this.bank.releaseResource(ID, current))sleep(500);} catch (InterruptedException e) {}
	}
}