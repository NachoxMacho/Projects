import java.util.concurrent.ThreadLocalRandom;

/**
 * @brief Uses the BankInterface to make a bank with customers
 * @see BankInterface
 * @version 1.3
 * @author Rob Owens
 */


public class Bank implements BankInterface{
	int totalCustomers; // Set by the users cmd line arguments
	int totalResources; // Set by the users cmd line arguments

	int[] available; // Vector used to store the current available resources
	int[] max; // The maximum amount of resources generated by the system
	int[][] current; // The current allocation matrix
	int[][] customerMax; // The matrix with the maximum of each resource each customer can ask for.
	Thread[] customerThreads;
	
	/**
	 * @brief Accepts the arguments and passes them to the bank class
	 * @param args - The Command line arguments, only the first 2 are used and in order (Customers Resources)
	 * @return void
	 * @author Rob Owens
	 */

	public static void main(String[] args) {
		int customers = Integer.parseInt(args[0]); // Converts the first String argument to an integer (Customers)
		int resources = Integer.parseInt(args[1]); // Converts the second String argument to an integer (Resources)
		new Bank(customers, resources); //Creates the bank object		
	}
	/**
	 * @brief Constructor for Bank, creates vectors and matrixes,
	 * Also, generates random values for the max resources and how much each customer can request
	 * @param Customers - The Number of Customers the bank has
	 * @param Resources - The Number of Resources the bank has
	 * @author Rob Owens
	 */
	public Bank(int Customers, int Resources) {
		// Sets the banks customers and resource totals
		totalCustomers = Customers;
		totalResources = Resources;
		
		// Create the arrays to the sizes required
		max = new int[Resources]; 
		available = new int[Resources];
		current = new int[Customers][Resources];
		customerMax = new int[Customers][Resources];
		
		// Generates the random number of resources to give to the bank
		for(int i = 0; i < totalResources; i++) {
			max[i] = available[i] = ThreadLocalRandom.current().nextInt(1, 10);
		}
		
		// Sets the allocation and needs matrixes to be 0s
		for(int i = 0;i < totalCustomers; i++) {
			for(int j = 0; j < totalResources; j++) {
				current[i][j] = 0;
				customerMax[i][j] = ThreadLocalRandom.current().nextInt(0, max[j]);
			}
		}
		// Outputs the starting state of the bank, before customers are created
		// Outputs the array Max
		System.out.print("Maximum:   [ ");
		for(int i = 0; i < this.totalResources; i++)
			System.out.print(this.max[i] + " ");
		System.out.print("]\n");
		
		this.outState(); 
		// Outputs the array Current
		System.out.print("Customer Max Demands:\n");
		for(int i = 0; i < this.totalCustomers; i++) {
			System.out.print("[ ");
			for(int j = 0; j < this.totalResources; j++)
				System.out.print(this.customerMax[i][j] + " ");
			System.out.print("]\n");
		}
		
		// Creates an array to store the threads of the customers
		this.customerThreads = new Thread [Customers]; 
		
		//Creates the customer threads and starts them
		for(int i = 0; i < this.customerThreads.length; i++) {
			this.customerThreads[i] = new Thread(new RunCustomer(i, Resources, this));
			this.customerThreads[i].start();
		}
	}
	
	/* (non-Javadoc)
	 * @see BankInterface#requestResource(int, int[])
	 */
	public synchronized boolean requestResource(int customerID, int[] incomingNeeds) throws InterruptedException{
		
		while(!this.getSafeState( customerID, incomingNeeds)) {
			System.out.print("ID#"+customerID+" is waiting.\n");
			wait();
		}
		
		for(int i = 0; i < totalResources; i++) {
			current[customerID][i] += incomingNeeds[i];
			available[i] -= incomingNeeds[i];
		}
		this.outState();
		return true;
	}

	
	/* (non-Javadoc)
	 * @see BankInterface#releaseResource(int, int[])
	 */
	public synchronized boolean releaseResource(int customerID, int[] returns) throws InterruptedException{
		for(int i = 0; i < totalResources; i++) {
			current[customerID][i] -= returns[i]; // Remove the number of resources from that users allocation
			available[i] += returns[i]; // Add the resource back to available vector
		}

		this.outState();
		notifyAll();
		return true;
	}

	// -------------------------------------------------------------------
	// TODO: This needs to get reworked with more foresight
	// According to the sample output there should be a sequence? Not sure what this means
	// Makes the system unreliable at higher numbers, try 10 and 10 as arguments to verify that it works
	// Once this is fixed everything else should work... I think
	// Look at page 332 in book for algorithm that needs to be implemented (7.5.3.1/2)
	// -------------------------------------------------------------------
	// THIS IS VERY BASIC FORM, NEEDS TO BE CHANGED 
	/*public synchronized boolean SafeState(int[] needs) {
			boolean adequateResources = true;

			for(int j = 0; j < totalResources; j++) { 
				if(needs[j] > this.available[j])
					adequateResources = false;
		}
		return adequateResources;
		}*/

	public synchronized boolean getSafeState(int customerID,int[] needs) {
			boolean adequateResources = true;

			for(int j = 0; j < totalResources; j++) {
				if(needs[j] > this.available[j]){
					adequateResources = false;
				}
					//else 
						//adequateResources = true;
		
}

		if (adequateResources==true){
		System.out.print(" Bank- Safe Sequence:\n[");
		for(int j = 0; j < totalResources; j++) { 
			System.out.print(j+" ");
		}
		System.out.print("]\n");
		System.out.print("Customer " + customerID + " Request  granted.\n");
		
return true;

}
else if(adequateResources==false)
{
	System.out.print("Bank - Safe sequence not found \n");
return false;
} 
else 
return true;
}


/*
		//Tests if needs is asking for more resources then the system has
		//if(needs[j] < this.available[j])
		System.out.print(" Bank- Safe Sequence:\n[");
		for(int j = 0; j < totalResources; j++) { 
			if(needs[j] < this.available[j])
			System.out.print(j+" ");
return true;
		}

        System.out.print("]\n");
        //System.out.print("Customer  Request " + Bank.requests + " granted.\n");
      
			
						

					
					return false;

					//System.out.print("Bank- Safe Sequence not found ");

			}
		
	*/

	/* (non-Javadoc)
	 * @see BankInterface#outState()
	 */
	public void outState() {
		// Outputs the array Available
		System.out.print("Available: [ ");
		for(int i = 0; i < this.totalResources; i++)
			System.out.print(this.available[i] + " ");
		System.out.print("]\n");
		
		// Outputs the array Current
		System.out.print("Current Allocation:\n");
		for(int i = 0; i < this.totalCustomers; i++) {
			System.out.print("[ ");
			for(int j = 0; j < this.totalResources; j++)
				System.out.print(this.current[i][j] + " ");
			System.out.print("]\n");
		}
	}

}

