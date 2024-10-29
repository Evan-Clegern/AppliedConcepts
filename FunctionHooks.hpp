#ifndef ERC_FUNCHOOK
#define ERC_FUNCHOOK

#include <vector>

/********!
* @class c_FuncHook_Typed
* 
* @date 24 October 2024
* 
* @brief
* Provides a means of invoking multiple, extensible functions in recursive relation to one main function. Each "hooked"
* function is called, in order, by the invoke() method, and each hooked function is expected to invoke the next level of
* hook functions unless it has good reason NOT to, such as overriding a behavior. Functions must return a valid type for
* this class; for void-returning functions, use c_FuncHook_Void.
* 
* @details
* Each hooked function must return the same type as the original function, and must take the same parameters as the original
* function, except for their first one, which is instead a pointer to this Hook Handler. Each hooked function must invoke
* the next hook(s) or original function at some point in their execution, unless it is explicitly an override that prevents
* the original function from being called.
*
* A useful way of implementing could be with plugin- and polymorphism-extensible classes, giving each object the means
* to determine its intended behavior, and then having a function with the actual "act" call. The "Determine Intended"
* could then be registered wiht a c_FuncHookHandler to allow for the addition of hooks in the decision process, while
* still leaving the "carry out the decision" process untouched. Alternatively, the ACT function could be hooked as well,
* so long as ALL of the hooks registered for it ALWAYS invoked the next function. This would have to be done by programming
* rules alone, since the class has no means of telling if a hook function invokes the next or not.
*
* This could also go quite well with the BOOST library for DLL/SO loading during runtime, such that you could standardize
* how modules would interface with particular parts of your code. It would work well for keeping modules "constrained,"
* especially for open-source projects or for game modding, where you can't truly be confident in the stability of modules.
*
* @note
* Successful implementation of this system is largely a developer-dependent process regarding the design and stability of
* the hooked functions used. Due to the inherent nature of using function pointers, very little data safety is possible on
* the end of this class, but measures have been taken as best possible.
********/
template<typename Returns, typename... Params> class c_FuncHook_Typed {
public:
	//! Templated type-alias for valid function hooks.
	using HookForm = Returns (*)(c_FuncHook_Typed<Returns, Params...>* const, Params...);
private:
	std::vector<HookForm> registered_hooks;
	uint32_t current_hook = 0, max_hook = 0;
	Returns (*const original)(Params...);
public:
	/********!
	 * @date	24 October 2024
	 * @brief
	 *  	Constructs a templated c_FuncHook_Typed hook wrapper for the specified "original" function.
	 * @param [in] initial
	 *  	Function pointer to the original function for which hooks will be applied; function must take the same
	 * 		sequence of parameters as specified in the template parameter @c Params, and must return the same type as
	 * 		specified in the template parameter @c Returns, and must not be volatile or ephemeral.
	 ********/
	c_FuncHook_Typed(Returns (* const initial)(Params...)) : original(initial) {}
	
	/********!
	 * @date	24 October 2024
	 * @brief
	 *  	Registers the specified function as a hook to be executed when the object has its call() method called.
	 * @param [in] newhook
	 * 		Pointer to a valid, non-ephemeral function that takes a pointer to this object as its first parameter, followed
	 * 		by all of the packed parameters as specified in the template parameter @c Params, and which returns a non-null
	 * 		value of type @c Returns.
	 * @return
	 *		Returns TRUE if the pointer provided was non-null and could be added to the object's vector of hooks.
	 ********/
	bool addHook(const HookForm newhook) {
		if (newhook != nullptr) {
			registered_hooks.push_back(newhook);
			max_hook += 1;
			return true;
		}
		return false;
	}
	
	/********!
	 * @date	25 October 2024
	 * @brief
	 *  	Initiates the execution of all of the hooked functions, assuming that each level's hooked function calls the
	 *		invoke method at some point in execution. Has a failsafe to execute only the original function if an exception
	 *		is encountered.
	 * @param [in] parameters
	 *  	Packed parameters, as specified by the template parameter @c Params, to pass to all hooked functions.
	 * @return
	 *		Returns an output of type @c Returns as dependent on the hooked functions and original function.
	 * @note
	 *		If the try-catch encounters an exception, the default behavior is to execute the original function and nothing else.
	 ********/
	Returns call(Params... parameters) {
		try {
			current_hook = 0;
			Returns output = this->invoke(parameters...);
			return output;
		} catch (void* obj) {
			current_hook = 0;
			return this->original(parameters...);
		}
	}
	/********!
	 * @date	24 October 2024
	 * @brief
	 *  	Initiates the execution of the next pending hooked function, or the original function.
	 * @param [in] parameters
	 *  	Packed parameters, as specified by the template parameter @c Params, to pass to the next hooked function.
	 * @return
	 *		Returns an output of type @c Returns as dependent on the hooked functions and original function.
	 * @note
	 *		Do not use this to initialize the hooked execution. This method is intended for use within the hooked functions,
	 *		such that they can invoke the next level function as needed.
	 ********/
	Returns invoke(Params... parameters) {
		Returns value;
		if (current_hook != max_hook) {
			Returns (*callable)(c_FuncHook_Typed<Returns, Params...>* const, Params...) = this->registered_hooks.at(current_hook);
			current_hook++;
			value = callable(this, parameters...);
		} else {
			value = this->original(parameters...);
		}
		return value;
	}
};


/********!
* @class c_FuncHook_Void
* 
* @date 25 October 2024
* 
* @brief
* Provides a means of invoking multiple, extensible functions in recursive relation to one main function. Each "hooked"
* function is called, in order, by the invoke() method, and each hooked function is expected to invoke the next level of
* hook functions unless it has good reason NOT to, such as overriding a behavior. Functions cannot return any values.
********/
template<typename... Params> class c_FuncHook_Void {
public:
	//! Templated type-alias for valid function hooks.
	using HookForm = void (*)(c_FuncHook_Void<Params...>* const, Params...);
private:
	std::vector<HookForm> registered_hooks;
	uint32_t current_hook = 0, max_hook = 0;
	void (* const original)(Params...);
public:
	/********!
	 * @date	25 October 2024
	 * @brief
	 *  	Constructs a templated c_FuncHook_Void hook wrapper for the specified "original" function.
	 * @param [in] initial
	 *  	Function pointer to the original function for which hooks will be applied; function must take the same
	 * 		sequence of parameters as specified in the template parameter @c Params, and must be a VOID-returning function
	 * 		that is non-volatile and non-ephemeral.
	 ********/
	c_FuncHook_Void(void (* const initial)(Params...)) : original(initial) {}
	
	/********!
	 * @date	25 October 2024
	 * @brief
	 *  	Registers the specified function as a hook to be executed when the object has its call() method called.
	 * @param [in] newhook
	 * 		Pointer to a valid, non-ephemeral function that takes a pointer to this object as its first parameter, followed
	 * 		by all of the packed parameters as specified in the template parameter @c Params, and which has no return value.
	 * @return
	 *		Returns TRUE if the pointer provided was non-null and could be added to the object's vector of hooks.
	 ********/
	bool addHook(const HookForm newhook) {
		if (newhook != nullptr) {
			registered_hooks.push_back(newhook);
			max_hook += 1;
			return true;
		}
		return false;
	}
	/********!
	 * @date	25 October 2024
	 * @brief
	 *  	Initiates the execution of all of the hooked functions, assuming that each level's hooked function calls the
	 *		invoke method at some point in execution. Has a failsafe to execute only the original function if an exception
	 *		is encountered.
	 * @param [in] parameters
	 *  	Packed parameters, as specified by the template parameter @c Params, to pass to all hooked functions.
	 * @note
	 *		If the try-catch encounters an exception, the default behavior is to execute the original function and nothing else.
	 ********/
	void call(Params... parameters) {
		try {
			current_hook = 0;
			this->invoke(parameters...);
		} catch (void* obj) {
			current_hook = 0;
			this->original(parameters...);
		}
		return;
	}
	/********!
	 * @date	25 October 2024
	 * @brief
	 *  	Initiates the execution of the next pending hooked function, or the original function.
	 * @param [in] parameters
	 *  	Packed parameters, as specified by the template parameter @c Params, to pass to the next hooked function.
	 * @note
	 *		Do not use this to initialize the hooked execution. This method is intended for use within the hooked functions,
	 *		such that they can invoke the next level function as needed.
	 ********/
	void invoke(Params... parameters) {
		if (current_hook != max_hook) {
			void (*callable)(c_FuncHook_Void<Params...>* const, Params...) = this->registered_hooks.at(current_hook);
			current_hook++;
			callable(this, parameters...);
		} else {
			this->original(parameters...);
		}
		return;
	}
};

/********!
* @example
* The following code is an example use of the FuncHookHandler with two hooked functions executing in a different order.
* Hook1 invokes the *next* hook when it returns, so after its main execution completes.
* Hook2 invokes the *next* hook before it returns, so before its main execution completes.
* With that in mind, the return order for this code is Hook1 -> MainFunction -> Hook2.
* @code
#include <iostream>
#include "functionHooks.hpp"

int MainFunction(int A, int B) {
	std::cout << "Main Function called.\n";
	return (2 * A) + B + 1;
}
c_FuncHookHandler<int, int, int> MainFunctionHooks(MainFunction);

int Hook1(c_FuncHookHandler<int, int, int>* const orig, int A, int B) {
	std::cout << "Hook 1 called with " << A << " and " << B << '\n';
	return orig->invoke(A + 2, B);
}
int Hook2(c_FuncHookHandler<int, int, int>* const orig, int A, int B) {
	int tmp = orig->invoke(A, B);
	std::cout << "Hook 2 called with " << A << " and " << B << " and output " << tmp << '\n';
	if (tmp == 12) {
		return 0;
	} else {
		return tmp + A + B;
	}
}

int main() {
	MainFunctionHooks.addHook(Hook1);
	MainFunctionHooks.addHook(Hook2);
	int output = MainFunctionHooks.call(1, 1); // should return 12
	std::cout << "Final output from the (1, 1) hook series: " << output << '\n';
	output = MainFunctionHooks.call(3, 1); // should return 0
	std::cout << "Final output from the (3, 1) hook series: " << output << '\n';
	return 0;
}

* @example
* The following code is an example of more advanced hooked function behavior, wherein a hook can be used to override or
* otherwise prevent execution of downstream invocations, prematurely (though with reason) terminating the recursive invoke.
* In the case of (1, 1), it executes Hook1 -> Hook2 -> Hook3 -> MainFunction.
* In the case of (3, 5), it executes Hook1 -> Hook2.
* @code
#include <iostream>
#include "functionHooks.hpp"

int MainFunction(int A, int B) {
	std::cout << "Main Function called.\n";
	return (2 * A) + B + 1;
}
c_FuncHookHandler<int, int, int> MainFunctionHooks(MainFunction);

int Hook1(c_FuncHookHandler<int, int, int>* const orig, int A, int B) {
	std::cout << "Hook 1 called with " << A << " and " << B << '\n';
	return orig->invoke(A + 2, B);
}
int Hook2(c_FuncHookHandler<int, int, int>* const orig, int A, int B) {
	std::cout << "Hook 2 called with " << A << " and " << B << '\n';
	if (A == B) {
		return 7;
	}
	return orig->invoke(A, 2 * B);
}
int Hook3(c_FuncHookHandler<int, int, int>* const orig, int A, int B) {
	std::cout << "Hook 3 called with " << A << " and " << B << '\n';
	return orig->invoke(A + 1, B + 1);
}

int main() {
	MainFunctionHooks.addHook(Hook1);
	MainFunctionHooks.addHook(Hook2);
	MainFunctionHooks.addHook(Hook3);
	int output = MainFunctionHooks.call(1, 1); // Returns 12, and it reaches MainFunction
	std::cout << "Final output from the hook madness: " << output << '\n';
	output = MainFunctionHooks.call(3, 5); // Returns 7, and does not reach MainFunction
	std::cout << "Final output from the hook madness: " << output << '\n';
	return 0;
}
*/

#endif
