# Venora

A client/server implementation of an active exploitation test tool

This is an open-source solution for organizing and trying out various exploits in the wild and serves as the backbone of a more advanced application in a private vulnerability test-suite
Note that due to security, the exploit packages are not publicly available. Nonetheless, this should be enough of a playground to serve as the infrastructure if you did want to play around with your own.

Disclaimer: The software provided here is for educational purposes only. Any use of this software is at your own risk. I make no warranties or representations regarding the accuracy, reliability, or completeness of the software or its output. By using this software, you agree that we shall not be liable for any damages or losses arising from your use of or reliance on the software. Please use this software responsibly and in accordance with all applicable laws and regulations.

## Usage

First: `docker-compose build`

<br>
Then open a couple of terminals
<br>

tty1: `docker-compose up server`
This will load the server's dependent services (db, db-accessor-api)

tty2: `docker-compose up client-frontend`
This will also load all dependent services (client-backend)

## Architecture

### Frontend (React)

- Establish WebSocket connection to Web Backend API.
- Receive real-time updates (target status, connectivity).
- Send user commands to update or act on targets.

### Web Backend API (Flask)

- Manages WebSocket connections for clients.
- Maintains dedicated TCP socket connections to the app server.
- Uses Redis to:
  - Cache WebSocket-to-TCP socket mappings.
  - Cache target statuses for periodic database writes.
  - Publish/subscribe to messages for efficient inter-component communication.
- Pushes updates to WebSocket clients when the app server sends status changes.

### App Server (C)

- Communicates with targets.
- Publishes connectivity or state changes to Redis.
- Subscribes to commands from the Web Backend API (e.g., via Redis Pub/Sub).

### Redis

- Stores WebSocket-to-TCP socket mappings for routing.
- Caches frequently updated target statuses.
- Acts as a Pub/Sub broker between the Web Backend API and the App Server.

### Database Backend API (Flask)

- Periodically syncs Redis caches to the Postgres database using Celery tasks.

### Celery

- Performs background tasks, such as flushing Redis caches to the database or handling long-running jobs.

## Summary

Venora utilizes a modern tech stack to provide a robust and scalable solution for active exploitation testing. The frontend is built with React, ensuring a dynamic and responsive user interface. The backend leverages Flask for the web API, with JWT-extended for secure authentication, SocketIO for real-time communication, Celery for task scheduling, and Redis for efficient caching. The app server is powered by Crver and json-c, with SSL for secure connections. The database API is implemented using Python Flask and JWT, with Redis for caching, and the primary database is Postgres, known for its reliability and performance.

![Tech Stack](path/to/tech-stack-image.png)
