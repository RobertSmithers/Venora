import axios from "axios";
import { useState } from "react";

import "./SignInForm.css";

export default function SignInForm() {
  // const [rememberMe, setRememberMe] = useState(false);
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [confirmPassword, setConfirmPassword] = useState("");
  const [errorMessage, setErrorMessage] = useState("");
  const [isSignUp, setIsSignUp] = useState(false);

  // const handleSetRememberMe = () => setRememberMe(!rememberMe);

  const handleSubmit = async (e) => {
    e.preventDefault();
    setErrorMessage(""); // Clear any previous error messages

    if (isSignUp && password !== confirmPassword) {
      setErrorMessage("Passwords do not match!");
      return; // Stop further execution if there's an error
    }

    try {
      const response = await axios.post("/register", { username, password });
      console.log("Registration success:", response.data);
      // Handle successful registration (e.g., redirect to login page)
    } catch (error) {
      if (error.response) {
        // Server responded with a status other than 200 range
        switch (error.response.status) {
          case 401:
            setErrorMessage(
              "Unauthorized: Web App must connect with server first (reload the page)."
            );
            break;
          case 500:
            setErrorMessage("Server error (is it online?): Please try again later.");
            break;
          default:
            setErrorMessage(`Error: ${error.response.status} - ${error.response.data.message}`);
        }
      } else if (error.request) {
        // Request was made but no response received
        setErrorMessage("Network error: Unable to connect to the server.");
      } else {
        // Something else happened while setting up the request
        setErrorMessage(`Error: ${error.message}`);
      }
    }
  };

  return (
    <div className={`form-container ${isSignUp ? "signup-mode" : ""}`}>
      <form className="auth-form" onSubmit={handleSubmit}>
        <h2>{isSignUp ? "Sign Up" : "Login"}</h2>
        <div className="input-container">
          <input
            type="text"
            placeholder="Username"
            value={username}
            onChange={(e) => setUsername(e.target.value)}
            required
          />
        </div>
        <div className="input-container">
          <input
            type="password"
            placeholder="Password"
            value={password}
            onChange={(e) => setPassword(e.target.value)}
            required
          />
        </div>
        {isSignUp && (
          <div className="input-container">
            <input
              type="password"
              placeholder="Confirm Password"
              value={confirmPassword}
              onChange={(e) => setConfirmPassword(e.target.value)}
              required
            />
          </div>
        )}
        <button type="submit" className="btn primary-btn">
          {isSignUp ? "Register" : "Login"}
        </button>
        <button
          type="button"
          className="btn toggle-btn"
          onClick={() => {
            setIsSignUp(!isSignUp);
            setErrorMessage("");
          }}
        >
          {isSignUp ? "Already have an account? Login" : "Don't have an account? Sign Up"}
        </button>

        {errorMessage && (
          <div className="error-message" style={{ color: "red", marginTop: "10px" }}>
            {errorMessage}
          </div>
        )}
      </form>
    </div>
  );
}
