
/**
 * @brief Provides an interface for a bank class with customers that can request and return resources.
 * Completely synchronized and doesn't enter deadlock states
 * @version 1.0
 * @author Rob Owens
 */
public interface BankInterface {

	// Customer request resources 
	/**
	 * @brief Synchronized method for customers getting resources. Prevents deadlock with getSafeState();
	 * @see BankInterface#getSafeState(int[]);
	 * @param customerID - The ID of the customer making the request
	 * @param needs - The resource requests of the customer
	 * @return true - When The request is granted
	 * @return false - Never
	 * @author Rob Owens
	 */
	public boolean requestResource(int customerID, int[] needs) throws InterruptedException;
	
	// Customer returns resources 
	/**
	 * @brief Combines the returns vector with the available vector and updates the allocation matrix
	 * @param CustomerID - The ID of the customer making the request
	 * @param returns - The resources the customer is returning 
	 * @return true - When The request is granted
	 * @return false - Never
	 * @throws InterruptedException if it will send the system into deadlock
	 * @author Rob Owens
	 */
	public boolean releaseResource(int CustomerID, int[] returns) throws InterruptedException;
	
	// Determines if the system is in a safe state
	/**
	 * @brief Needs work, suppose to detect if needs[] will send the system into deadlock
	 * @param needs - The resource requests of the customer
	 * @return true - If the system can accept the requests of the customer without going into deadlock
	 * @return false - If the request would send the system into deadlock
	 * @throws InterruptedException if it will send the system into deadlock
	 * @author Rob Owens
	 */
	public boolean getSafeState(int customerID, int[] needs);
	
	// Outputs the current State of the system
	/**
	 *  @brief Outputs available[] and current[], used for output and debugging
	 *  @author Rob Owens
	 */

	public void outState();
}
