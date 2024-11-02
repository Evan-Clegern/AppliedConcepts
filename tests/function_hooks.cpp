#include <iostream>
#include "AppliedConcepts/FunctionHooks.hpp"


int MainFunction(int A, int B) {
	std::cout << "Main Function called.\n";
	return (2 * A) + B + 1;
}
c_FuncHook_Typed<int, int, int> MainFunctionHooks(MainFunction);

int Hook1(c_FuncHook_Typed<int, int, int>* const orig, int A, int B) {
	std::cout << "Hook 1 called with " << A << " and " << B << '\n';
	return orig->invoke(A + 2, B);
}
int Hook2(c_FuncHook_Typed<int, int, int>* const orig, int A, int B) {
	std::cout << "Hook 2 called with " << A << " and " << B << '\n';
	if (A == B) {
		return 7;
	}
	return orig->invoke(A, 2 * B);
}
int Hook3(c_FuncHook_Typed<int, int, int>* const orig, int A, int B) {
	std::cout << "Hook 3 called with " << A << " and " << B << '\n';
	return orig->invoke(A + 1, B + 1);
}

void VHook1(c_FuncHook_Void<int, char>* const orig, int A, char B) {
	if (B == '\n') {
		orig->invoke(A, B);
		return;
	}
	std::cout << B << " is " << A << '\n';
	orig->invoke(A + 1, B);
}
void VHook2(c_FuncHook_Void<int, char>* const orig, int A, char B) {
	orig->invoke(A + B, B);
}
void VHook3(c_FuncHook_Void<int, char>* const orig, int A, char B) {
	if (B == '\n') {
		orig->invoke(3 * A, '!');
	} else {
		orig->invoke(A + 1, B);
	}
}
void VoidFunction(int A, char B) {
	if (A == 0 || A == B) {
		std::cout << "Got zero or a tie. Nothing cool happened.\n";
	} else if (A < B) {
		std::cout << B << " beats " << A << '\n';
	} else {
		std::cout << A << " beats " << B << '\n';
	}
}
c_FuncHook_Void<int, char> VoidFunctionHooks(VoidFunction);

int main() {
	MainFunctionHooks.addHook(Hook1);
	MainFunctionHooks.addHook(Hook2);
	MainFunctionHooks.addHook(Hook3);
	VoidFunctionHooks.addHook(VHook1);
	VoidFunctionHooks.addHook(VHook2);
	VoidFunctionHooks.addHook(VHook3);
	int output = MainFunctionHooks.call(1, 1);
	std::cout << "Final output from the hook madness: " << output << '\n';
	output = MainFunctionHooks.call(3, 5);
	std::cout << "Final output from the hook madness: " << output << "\n\n";
	VoidFunctionHooks.call(10, 'A');
	VoidFunctionHooks.call(10, 'C');
	VoidFunctionHooks.call(10, '\n');
	return 0;
}
