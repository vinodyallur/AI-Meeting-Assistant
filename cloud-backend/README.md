# Cloud Backend - AI Meeting Assistant

This is the cloud backend service for the AI Meeting Assistant system. It handles audio streaming, image uploads, speech-to-text conversion, and AI-powered meeting summarization.

## Architecture

The backend consists of multiple microservices:

1. **API Service** - Handles HTTP requests from ESP32 devices
2. **Worker Service** - Processes async AI tasks (STT, summarization)
3. **Redis** - Session management and job queue
4. **Monitoring** - Prometheus + Grafana for observability

## Features

- **Real-time audio streaming** with chunked transfer encoding
- **Image upload** with metadata (device MAC, camera ID)
- **Azure Speech-to-Text** integration for transcription
- **OpenAI GPT-4** for meeting summarization
- **Azure Blob Storage** for recording persistence
- **WebSocket support** for real-time updates
- **RESTful API** for device communication
- **Health monitoring** and metrics

## API Endpoints

### Audio Streaming
```
POST /api/upload
Query Parameters:
  - filename: Name of the audio file (e.g., live.wav)
  - mac_address: Device MAC address
  
Headers:
  - Content-Type: audio/wav
  - Transfer-Encoding: chunked
```

### Image Upload
```
POST /api/upload_image
Content-Type: multipart/form-data

Form Data:
  - mac_address: Device MAC address
  - camera_id: Camera identifier (CAM_1, CAM_2)
  - file: JPEG image file
```

### Meeting Control
```
POST /api/end_session_by_mac
Query Parameters:
  - mac_address: Device MAC address
  
Triggers AI processing for completed meeting
```

### Health Check
```
GET /health
Returns service health status
```

## Setup Instructions

### Prerequisites
- Docker and Docker Compose
- Azure account with:
  - Storage Account (Blob Storage)
  - Cognitive Services (Speech)
  - OpenAI API access
- Node.js 18+ (for development)

### Environment Variables

Create `.env` file:
```bash
# Azure Configuration
AZURE_STORAGE_CONNECTION_STRING="DefaultEndpointsProtocol=https;AccountName=..."
AZURE_SPEECH_KEY="your-speech-key"
AZURE_SPEECH_REGION="centralindia"

# OpenAI Configuration
OPENAI_API_KEY="sk-..."
OPENAI_MODEL="gpt-4"

# Application Configuration
NODE_ENV="production"
PORT=8080
LOG_LEVEL="info"

# Redis
REDIS_URL="redis://redis:6379"

# Grafana
GRAFANA_PASSWORD="admin123"
```

### Deployment

#### Local Development
```bash
cd cloud-backend
npm install
npm run dev
```

#### Docker Deployment
```bash
cd docker
docker-compose up -d
```

#### Azure Container Apps
```bash
# Build and push to Azure Container Registry
az acr build --registry yourregistry \
  --image meeting-backend:latest .

# Deploy to Container Apps
az containerapp create \
  --name meeting-backend \
  --resource-group your-rg \
  --image yourregistry.azurecr.io/meeting-backend:latest \
  --target-port 8080 \
  --ingress external \
  --env-vars \
    AZURE_STORAGE_CONNECTION_STRING=$AZURE_STORAGE_CONNECTION_STRING \
    AZURE_SPEECH_KEY=$AZURE_SPEECH_KEY \
    OPENAI_API_KEY=$OPENAI_API_KEY
```

## Directory Structure

```
cloud-backend/
├── src/
│   ├── api/           # Express API routes
│   ├── services/      # Business logic
│   ├── models/        # Data models
│   ├── utils/         # Helper functions
│   └── workers/       # Background job processors
├── tests/             # Unit and integration tests
├── docker/            # Docker configuration
├── .env.example       # Environment template
├── package.json
├── Dockerfile         # Production Dockerfile
├── Dockerfile.worker  # Worker service Dockerfile
└── README.md
```

## Development

### Running Tests
```bash
npm test              # Run all tests
npm run test:unit     # Run unit tests
npm run test:integration  # Run integration tests
```

### Code Style
```bash
npm run lint          # ESLint check
npm run format        # Prettier formatting
```

### Database Migrations
```bash
npm run migrate:up    # Run migrations
npm run migrate:down  # Rollback migrations
npm run migrate:create # Create new migration
```

## Monitoring

### Metrics
The service exposes Prometheus metrics at `/metrics`:
- HTTP request duration and count
- Audio processing latency
- Image upload success rate
- AI processing queue length

### Logging
Structured JSON logging with log levels:
- `error`: Critical failures
- `warn`: Non-critical issues
- `info`: General information
- `debug`: Detailed debugging

### Health Checks
- API service: `GET /health`
- Redis connection
- Azure services connectivity
- Disk space and memory

## Security

### Authentication
- API key authentication for device registration
- JWT tokens for admin access
- Rate limiting per device

### Data Protection
- Audio recordings encrypted at rest
- Secure transmission (HTTPS only)
- Regular security audits
- Vulnerability scanning

### Compliance
- GDPR compliance for personal data
- Data retention policies
- Access logging and audit trails

## Scaling

### Horizontal Scaling
- Stateless API services
- Redis for session storage
- Message queue for async processing

### Performance Optimization
- Audio stream buffering
- Image compression
- Connection pooling
- Query optimization

### Cost Optimization
- Azure reserved instances
- Storage tier optimization
- Monitoring and alerting for cost spikes

## Troubleshooting

### Common Issues

1. **Audio Streaming Fails**
   - Check chunked transfer encoding
   - Verify WAV header format
   - Test with smaller chunks

2. **Image Upload Timeout**
   - Increase timeout settings
   - Optimize image size
   - Check network connectivity

3. **AI Processing Slow**
   - Monitor worker queue
   - Scale worker instances
   - Check API rate limits

4. **Storage Issues**
   - Verify Azure credentials
   - Check container permissions
   - Monitor storage capacity

### Debug Mode
Enable debug logging:
```bash
LOG_LEVEL=debug npm start
```

Check logs for detailed error information.

## Support

For issues and questions:
1. Check the [GitHub Issues](https://github.com/your-org/ai-meeting-assistant/issues)
2. Review API documentation
3. Contact development team with logs and error details