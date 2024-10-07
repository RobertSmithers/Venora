import React, { useEffect, useState } from "react";
import PropTypes from "prop-types";
import "./ConnectionError.css"; // Ensure to create a CSS file for styling

const ConnectionError = ({ onRetry }) => {
  const [countdown, setCountdown] = useState(10); // Set initial countdown (in seconds)
  //   const [retrying, setRetrying] = useState(false);
  const [retryMessage, setRetryMessage] = useState("");

  useEffect(() => {
    let timer;

    if (countdown === 0) {
      const attemptRetry = async () => {
        const success = await onRetry(); // Wait for retry result
        setRetryMessage(success ? "Connection successful!" : "Connection failed. Retrying...");

        if (retryMessage.includes("Connection failed")) {
          timer = setTimeout(() => {
            console.log("Clearing retry message...");
            setRetryMessage("");
          }, 3000);
        }
      };
      attemptRetry();
      setCountdown(10); // Reset countdown after retry
    } else {
      // Countdown timer
      const timer = setInterval(() => {
        setCountdown((prev) => prev - 1);
      }, 1000);
      return () => clearInterval(timer); // Cleanup on component unmount
    }

    return () => clearTimeout(timer);
  }, [countdown, onRetry]);

  const handleRetryClick = async () => {
    setCountdown(10); // Reset the countdown
    const success = await onRetry(); // Attempt to retry immediately
    setRetryMessage(success ? "Connection successful!" : "Connection failed. Please try again.");
  };

  return (
    <div className="connection-error">
      <h2>Can&apos;t Connect to the Server</h2>
      <p>Please check your internet connection or try again later.</p>
      <div className="countdown">
        <p>Retrying in {countdown} seconds...</p>
      </div>
      {retryMessage && <p>{retryMessage}</p>} {/* Display the retry message */}
      <button className="retry-button" onClick={handleRetryClick}>
        Retry Now
      </button>
    </div>
  );
};

// Prop validation
ConnectionError.propTypes = {
  onRetry: PropTypes.func.isRequired, // Expecting a function to be passed for retrying
};

export default ConnectionError;
